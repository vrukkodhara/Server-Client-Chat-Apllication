FROM ubuntu:latest

WORKDIR /main

COPY tcpServer.cpp .

# Update the packages in Ubuntu
RUN apt-get update && \
    apt-get install -y g++

# Compile the C++ file
RUN g++ -o tcpServer tcpServer.cpp

CMD ["./tcpServer"]
