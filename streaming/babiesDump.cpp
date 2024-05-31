#include <iostream>
#include <fstream>
#include <vector>
// #include <random>
// #include <future>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <librdkafka/rdkafka.h>
#include "mira_streaming.hpp"

// Global variables for Kafka (assuming they are defined somewhere else)
bool kafka_ready = false; // Placeholder

void babiesdump(int efn, rd_kafka_topic_t *topic)
{
    std::cout << "babiesdump thread starting" << std::endl;

    int port = 17601;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 1) < 0)
    {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "babiesdump: connected by " << inet_ntoa(address.sin_addr) << std::endl;

    int recvdata;
    if (recv(new_socket, &recvdata, sizeof(recvdata), MSG_WAITALL) != sizeof(recvdata))
    {
        perror("recv");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (recvdata == efn)
    {
        std::cout << "babiesdump: efn (" << efn << ")" << std::endl;
        int sendbuf = 1;
        if (send(new_socket, &sendbuf, sizeof(sendbuf), 0) != sizeof(sendbuf))
        {
            perror("send");
            close(new_socket);
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        while (true)
        {
            int recvlen;
            if (recv(new_socket, &recvlen, sizeof(recvlen), MSG_WAITALL) != sizeof(recvlen))
            {
                perror("recv");
                break;
            }
            std::cout << "babiesdump: " << recvlen << " bytes received" << std::endl;
            if (recvlen > 0)
            {
                std::vector<char> recvbuf(recvlen);
                if (recv(new_socket, recvbuf.data(), recvlen, MSG_WAITALL) != recvlen)
                {
                    perror("recv");
                    break;
                }
                std::cout << "babiesdump: " << recvlen << " bytes received" << std::endl;

                if (kafka_ready)
                {
                    // Placeholder for Kafka producer logic
                    mira::produce(topic, recvlen, recvbuf.data());
                }
            }
            else if (recvlen == 0)
            {
                continue;
            }
            else
            {
                std::cout << "babiesdump: babies stop ack (-1) received" << std::endl;
                break;
            }
        }
    }
    else
    {
        std::cout << "babiesdump: invalid efn (" << efn << ")" << std::endl;
    }

    close(new_socket);
    close(server_fd);

    std::cout << "babiesdump thread terminating" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: babiesDump [efn] [bootstrap_servers]" << std::endl;
        return 1;
    }
    int efn = std::atoi(argv[1]);           // efn
    std::string bootstrap_servers(argv[2]); // Kafka bootstrap.servers
    auto topic = mira::init_producer("0", bootstrap_servers, "rawdata");
    babiesdump(efn, topic);
    return 0;
}