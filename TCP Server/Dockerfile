FROM ubuntu:latest
WORKDIR /main

# Update the packages in Ubuntu
RUN apt-get update && apt-get install -y --fix-missing g++

# Copy and compile the C++ file
COPY tcpServer.cpp .
RUN g++ -o tcpServer tcpServer.cpp -pthread

# Set the command to run on container start
CMD ["./tcpServer"]
