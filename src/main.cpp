#include "Server.hpp"
#include "HTTPResponse.hpp"
#include "ServerException.hpp"
#include "Serializer.hpp"
#include "Logger.hpp"

int main() {
    try {
        
        //TO DO
        //1. Modificat clasele de response si request (membri privati) $$$$$$$$$$
        //2. Gestionarea erorilor cu cod eroare ex 404 si exceptii 
        //3. Defineuri pentru tipurile de content $$$$$$$$$$
        //4. Middleware (functii care se executa inainte de a ajunge la handler)
        //5. Posibilitatea de a lucra cu un server asincron 
        //6. Conectari cu diferite tipuri de baze de date (mysql, sqlite, etc)
        //7. Logger -> sa punem in fisiere logurile (adica erori si actiune normale in server) $$$$$$$$$$

        Server* my_server = new Server(true);


        my_server->__add_route__("/", "GET", [](const HttpRequest& req, HttpResponse& res) {
            
            res.setContentType(ContentType::HTML);
            res.SendHtmlPage("just_the_root.html");
            
        });
        

        my_server->__add_route__("/main", "GET", [](const HttpRequest& req, HttpResponse& res) {
            
            res.setContentType(ContentType::HTML);
            res.SendHtmlPage("main_page.html");
            
        });

        my_server->__add_route__("/reg", "GET", [](const HttpRequest& req, HttpResponse& res) {
            
            res.setContentType(ContentType::HTML);
            res.SendHtmlPage("register_page.html");
            
        });

        my_server->__add_route__("/save", "POST", [](const HttpRequest& req, HttpResponse& res) {
            
            printf("\n\n%s\n\n", req.toJson()["name"].get<std::string>().c_str());        
        });
        

        my_server->__add_route__("/users", "GET", [](const HttpRequest& req, HttpResponse& res) {
            
            nlohmann::json json_response = {
                {"users", {
                    {{"id", 1}, {"name", "John"}, {"email", "john@example.com"}},
                    {{"id", 2}, {"name", "Jane"}, {"email", "jane@example.com"}}
                }}
            };

             res.setContentType(ContentType::JSON); 
            res.Send(json_response);
            
        });
    
        my_server->start();

    } catch (const std::exception& e) {
        std::cerr << "Error starting the server: " << e.what() << std::endl;

        Logger* logger = Logger::getInstance();
        logger->enableConsoleOutput(false);
        logger->enableFileOutput("log.txt", true);
        logger->log(ERROR, e.what(), "main.cpp");
        
        return EXIT_FAILURE;
    }

    return 0;
}
