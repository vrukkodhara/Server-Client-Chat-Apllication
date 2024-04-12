#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

vector<int> clients;
mutex clientMutex;

void clientHandler(int clientSocket) {
    char clientMessage[1024] = {0};
    while (true) {
        int valread = read(clientSocket, clientMessage, 1024);

        if (valread <= 0) {
            close(clientSocket);

            lock_guard<mutex> guard(clientMutex);
            clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        }

        string broadcastMessage = "Client " + to_string(clientSocket) + ": " + clientMessage;

        // Broadcast message to all clients
        lock_guard<mutex> guard(clientMutex);
        for (int client : clients) {
            if (client != clientSocket) {
                send(client, broadcastMessage.c_str(), broadcastMessage.size(), 0);
            }
        }

        cout << "Client " << clientSocket << " : " << clientMessage << endl;
    }
}

void clientThreadHandler(int serverSocket) {
    int newSocket;
    struct sockaddr_in clientAddr;
    socklen_t addrSize;
    
    while (true) {
        newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);

        if (newSocket < 0) {
            cerr << "Error: Accept failed" << endl;
            continue;
        }

        lock_guard<mutex> guard(clientMutex);
        clients.push_back(newSocket);

        cout << "Client " << newSocket << " is connected" << endl;

        thread newHandler(clientHandler, newSocket);
        newHandler.detach();  // Detach each client handler thread
    }
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr; // Declare clientAddr here
    socklen_t addrSize;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error: Socket creation failed" << endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Use INADDR_ANY to bind to any available IP address
    serverAddr.sin_port = htons(54000); // Specify the port you want to use

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Bind failed" << endl;
        return -1;
    }

    if (listen(serverSocket, 5) < 0) {
        cerr << "Error: Listen failed" << endl;
        return -1;
    }

    addrSize = sizeof(clientAddr);
    cout << "Server started running" << endl;

    // Create and manage multiple client handling threads
    clientThreadHandler(serverSocket);

    return 0;
}
