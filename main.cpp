#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <jsoncpp/json/json.h>

int main(int argc, char **argv)
{
    // Generate Seed for random number generator
    srand((unsigned)time((time_t *)NULL));

    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error Creating Socket" << std::endl;
        return 1;
    }

    // Bind to Address and Port
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Use available network interface
    serverAddress.sin_port = htons(8080);       // Port 8080

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Error Binding Socket" << std::endl;
        return 1;
    }

    // Listen for Incoming Connections
    if (listen(serverSocket, 5) < 0)
    {
        std::cerr << "Error Listening on socket" << std::endl;
        return 1;
    }

    std::cout << "Server Listening on port 8080..." << std::endl;

    // Accept Incoming Connections and Handle Data
    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressSize);

        if (clientSocket < 0)
        {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        // Receive data from the client
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0)
        {
            std::cerr << "Error receiving data" << std::endl;
            close(clientSocket);
            continue;
        }

        // Process and handle the received data
        std::string receivedData(buffer, bytesRead);
        if (receivedData.find("OPTIONS") == 0)
        {
            // Handle CORS preflight request
            // Respond with CORS headers
            std::string corsResponse = "HTTP/1.1 200 OK\r\n";
            corsResponse += "Access-Control-Allow-Origin: *\r\n";
            corsResponse += "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n";
            corsResponse += "Access-Control-Allow-Headers: Content-Type\r\n";
            corsResponse += "Content-Length: 0\r\n\r\n";

            // Send CORS response
            send(clientSocket, corsResponse.c_str(), corsResponse.length(), 0);
        }
        if (receivedData.find("POST") == 0)
        {
            // Handle POST request

            // Extract JSON from HTTP request
            std::string jsonString;

            size_t jsonStart = receivedData.find("{");
            size_t jsonEnd = receivedData.find("}");

            if (jsonStart != std::string::npos && jsonEnd != std::string::npos)
            {
                jsonString = receivedData.substr(jsonStart, jsonEnd - jsonStart + 1);

                // std::cout << "Received Json Data: " << jsonString << std::endl;

                // Parse Json Data
                Json::Value obj;
                Json::Reader reader;

                if (reader.parse(jsonString, obj) < 0)
                {
                    std::cout << "Error Parsing Json Input" << std::endl;
                }

                std::string num = obj["variableValue"].asString();

                std::cout << "Slider Value: " << std::stoi(num) << std::endl;
            }
            else
            {
                std::cerr << "Error Extracting JSON\n";
            }

            // Send HTTP response
            std::string response = "HTTP/1.1 204 No Content\r\n";
            response += "Access-Control-Allow-Origin: *\r\n";
            response += "Content-Length: 0\r\n\r\n";

            send(clientSocket, response.c_str(), response.length(), 0);
        }
        if (receivedData.find("GET") == 0)
        {
            // Handle GET Request

            size_t pathStart = receivedData.find("GET ");
            if (pathStart != std::string::npos)
            {
                size_t pathEnd = receivedData.find(" HTTP/1.1", pathStart);
                if (pathEnd != std::string::npos)
                {
                    std::string requestedPath = receivedData.substr(pathStart + 4, pathEnd - pathStart - 4);
                    // std::cout << "Requested Path: " << requestedPath << std::endl;

                    // If the Requested Path Has '/data'
                    if (requestedPath == "/data")
                    {
                        // Prepare JSON String to send back
                        Json::Value jsonResponse;
                        jsonResponse["number"] = rand() % 100; // Generate Random Number Up To 100

                        // Data we want to send:
                        std::string responseData = jsonResponse.toStyledString();

                        // Construct HTTP Response
                        std::string httpResponse = "HTTP/1.1 200 OK\r\n";
                        httpResponse += "Access-Control-Allow-Origin: *\r\n";
                        httpResponse += "Content-Type: text/plain\r\n";
                        httpResponse += "Content-Length: " + std::to_string(responseData.size()) + "\r\n\r\n";
                        httpResponse += responseData;

                        // Send Data
                        send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);
                    }
                }
            }
        }

        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}