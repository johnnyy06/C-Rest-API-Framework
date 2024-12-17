#include "Server.hpp"
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory>
#include <fstream>
#include <sstream>
#include "ServerException.hpp"
#include "Logger.hpp"



Server::Server(bool useDynamicPool, int minThreads, int maxThreads, std::chrono::seconds idleTimeout, const int port)
    : m_port(port), m_server_socket(-1)
{
    Logger* logger = Logger::getInstance();
    logger->enableConsoleOutput(true);
    logger->enableFileOutput("log.txt", true);
    
    try {
        if (useDynamicPool) {
            logger->log(INFO, "Creating dynamic pool...", "Server.cpp");
            m_threadPool = std::make_unique<DynamicThreadPool>(minThreads, maxThreads, idleTimeout);
        } else {
            logger->log(INFO, "Creating fixed pool...", "Server.cpp");
            m_threadPool = std::make_unique<ThreadPool>(); 
        }
        
        initializeSocket(); 
    } catch (const SocketException& e) {
        std::cerr << "Failed to initialize server socket: " << e.what() << std::endl;
        logger->enableConsoleOutput(false);
        logger->log(ERROR, e.what(), "Server.cpp");
        exit(EXIT_FAILURE);  // Terminate if socket initialization fails
    }
}

void Server::start()
{
    Logger *logger = Logger::getInstance();
    logger->enableConsoleOutput(true);
    logger->enableFileOutput("log.txt", true);

    logger->log(INFO, "Server is listening on port " + std::to_string(m_port), "Server.cpp");

    while (true) {
        try {
            int client_socket = accept(m_server_socket, nullptr, nullptr);
            if (client_socket < 0) {
                throw SocketException("Accept failed");
            }
            logger->log(INFO, "Connection accepted on socket " + std::to_string(client_socket), "Server.cpp");

            m_threadPool->enqueue([this, client_socket]() {
                try {
                    handleRequest(client_socket);
                } catch (const RouterException& e) {
                    std::cerr << "Error handling request: " << e.what() << std::endl;
                }
            });


        } catch (const SocketException& e) {
            std::cerr << "Socket exception occurred: " << e.what() << std::endl;
            logger->enableConsoleOutput(false);
            logger->log(ERROR, e.what(), "Server.cpp");
        }
    }
}

void Server::__add_route__(const std::string &path, const std::string &method, Router::HandlerFunction handler)
{
    m_router.add_route(path, method, handler);
}

std::string Server::load_html_file(const std::string & filename)
{
    Logger *logger = Logger::getInstance();
    logger->enableConsoleOutput(false);
    logger->enableFileOutput("log.txt", true);

    try {
        std::ifstream file("html/" + filename);
        if (!file.is_open()) {
            throw FileException("Could not open file: " + filename);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();

    } catch (const FileException& e) {
        std::cerr << "File loading error: " << e.what() << std::endl;
        logger->log(ERROR, e.what(), "Server.cpp");
        
        return "<html><body><h1>404 Not Found</h1><p>Page not found.</p></body></html>";
    }
}

void Server::initializeSocket()
{
    m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_socket < 0) {
        throw SocketException("Socket creation failed!");
    }

    configureSocket();
    bindSocket();
    listenOnSocket();

    // sa fac catch pentru cele 3 functii de mai sus
}

void Server::configureSocket()
{
    int options = 1;
    if (setsockopt(m_server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options)) < 0) {
        close(m_server_socket);
        throw SocketException("Set socket options failed");
    }
}

void Server::bindSocket()
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(m_port);

    if (bind(m_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(m_server_socket);
        throw SocketException("Binding failed");
    }
}

void Server::listenOnSocket()
{
    if (listen(m_server_socket, 10) < 0) {
        close(m_server_socket);
        throw SocketException("Listen failed");
    }
}

void Server::handleRequest(int client_socket)
{
    Logger* logger = Logger::getInstance();
    logger->enableConsoleOutput(true);
    logger->enableFileOutput("log.txt", true);

    try {
        char buffer[1024] = {0};
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            close(client_socket);
            return;
        }
        std::stringstream ss;
        ss << std::this_thread::get_id();
        std::string thread_id_str = ss.str();

        logger->log(INFO, "Handling request on thread " + thread_id_str + ": " + buffer, "Server.cpp");

        std::string request_text(buffer, bytes_received);
        
        m_router.route_request(request_text, client_socket);

        close(client_socket);  // Close after responding

    } catch (const RouterException& e) {
        std::cerr << "Routing error: " << e.what() << std::endl;
        logger->enableConsoleOutput(false);
        logger->log(ERROR, e.what(), "Server.cpp");
        close(client_socket);
    }
}


