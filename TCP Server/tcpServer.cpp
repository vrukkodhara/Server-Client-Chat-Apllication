#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;

void InteractWithClient(int clientSocket, vector<int>& clients) {
    //send/recv client
    cout << "client connected " << endl;
    char buffer[1024];
    while (1) {
        int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesrecvd <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        string message(buffer, bytesrecvd);
        cout << "message from client : " << message << endl;

        for (auto& client : clients) {
            if (client != clientSocket) {
                send(client, message.c_str(), message.length(), 0);
            }
        }
    }
    auto it = find(clients.begin(), clients.end(), clientSocket);
    if (it != clients.end()) {
        clients.erase(it);
    }
    close(clientSocket);
}

int main() {
    cout << "server program" << endl;

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        cout << "Failed to create socket" << endl;
        return 1;
    }

    //address structure
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9090);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        cout << "Failed to bind socket" << endl;
        close(listenSocket);
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) < 0) {
        cout << "Failed to listen to client" << endl;
        close(listenSocket);
        return 1;
    }

    cout << "Server started running " << endl;
    vector<int> clients;
    while (1) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            cout << "Invalid client socket " << endl;
        }
        clients.push_back(clientSocket);
        thread t1(InteractWithClient, clientSocket, ref(clients));
        t1.detach();
    }

    close(listenSocket);

    return 0;
}
