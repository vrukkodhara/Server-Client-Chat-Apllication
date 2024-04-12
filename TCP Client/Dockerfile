FROM ubuntu:latest

# Install g++ compiler
RUN apt-get update && apt-get install -y g++

# Set the working directory
WORKDIR /main

# Copy the C++ source code into the container
COPY tcpClient.cpp .

# Compile the C++ code
RUN g++ -o tcpClient tcpClient.cpp

# Specify the command to run the compiled program
CMD ["./tcpClient"]
