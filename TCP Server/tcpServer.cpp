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

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error: Socket creation failed" << endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("172.17.0.2"); // Replace with the server IP
    serverAddr.sin_port = htons(54000);

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

    while (true) {
        newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);

        if (newSocket < 0) {
            cerr << "Error: Accept failed" << endl;
            continue;  // Move to the next iteration to continue accepting connections
        }

        lock_guard<mutex> guard(clientMutex);
        clients.push_back(newSocket);

        cout << "Client " << newSocket << " is connected" << endl;

        thread clientThread(clientHandler, newSocket);
        clientThread.detach();
    }

    return 0;
}
