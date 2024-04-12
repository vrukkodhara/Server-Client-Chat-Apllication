#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error: Socket creation failed" << endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = inet_addr("172.17.0.2"); // Replace with the server IP

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Connection failed" << endl;
        return -1;
    }

    cout << "Connected to server" << endl;
    
    char serverMessage[1024] = {0};
    while (true) {
        string message;
        cout << "Enter message: ";
        getline(cin, message);

        if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
            cerr << "Error: Send failed" << endl;
            continue;  // Move to the next iteration to allow re-sending of the message
        }

        int valread = read(clientSocket, serverMessage, 1024);
        if (valread < 0) {
            cerr << "Error: Read failed" << endl;
        }
        else {
            cout << serverMessage << endl;
        }
    }

    close(clientSocket);

    return 0;
}
