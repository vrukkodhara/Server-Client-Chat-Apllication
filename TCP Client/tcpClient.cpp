#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

using namespace std;

void SendMessages(int s) {
    cout << "Name your client : " << endl;
    string name;
    getline(cin, name);
    string message;
    while (1) {
        getline(cin, message);
        string msg = "( " + name + " ): " + message;
        int bytessent = send(s, msg.c_str(), msg.length(), 0);
        if (bytessent < 0) {
            cout << "error sending message " << endl;
            break;
        }
        if (message == "Q" || message == "q" || message == "quit" || message == "exit") {
            cout << name << " Stopped" << endl;
            break;
        }
    }
    close(s);
}

void ReceiveMessage(int s) {
    char buffer[1024];
    int recvlength;
    string msg = "";
    while (1) {
        recvlength = recv(s, buffer, sizeof(buffer), 0);
        if (recvlength <= 0) {
            cout << "disconnected from the server" << endl;
            break;
        } else {
            msg = string(buffer, recvlength);
            cout << msg << endl;
        }
    }
}

int main() {
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        cout << "invalid socket created " << endl;
        return 1;
    }

    int port = 9090;
    string serveraddress = "172.17.0.2";
    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, serveraddress.c_str(), &(servaddr.sin_addr));

    if (connect(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        cout << "Unable to connect to server" << endl;
        close(s);
        return 1;
    }
    cout << "Connected to server " << endl;

    thread senderthread(SendMessages, s);
    thread receiver(ReceiveMessage, s);

    senderthread.join();
    receiver.join();
    return 0;
}
