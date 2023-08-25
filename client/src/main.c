#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int port;
int client_socket;
struct pollfd pollfds[2];

char buffer[256];

int init_socket() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    return connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
}

void init_pollfds() {
    // stdin
    pollfds[0].fd = STDIN_FILENO;
    pollfds[0].events = POLLIN;

    // socket
    pollfds[1].fd = client_socket;
    pollfds[1].events = POLLIN;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        if (argc < 2)
            printf("%s", "Too few arguments supplied.\n");
        if (argc > 2)
            printf("%s", "Too many arguments supplied.\n");
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s", "Nuntium Client\n");

    port = atoi(argv[1]);

    if (init_socket() < 0) {
        printf("%s", "Couldn't connect to server\n");
        exit(EXIT_FAILURE);
    }

    init_pollfds();

    while (true) {
        int events = poll(pollfds, 2, 100);

        if (events == 0)
            continue;

        // stdin
        if (pollfds[0].revents & POLLIN) {
            fgets(buffer, sizeof(buffer), stdin);

            if (strcmp(buffer, "/exit\n\0") == 0)
                break;

            send(client_socket, buffer, sizeof(buffer), 0);
        }

        // socket
        if (pollfds[1].revents & POLLIN) {
            recv(client_socket, buffer, sizeof(buffer), 0);

            printf("%s", buffer);
        }
    }
    
    close(client_socket);

    return 0;
}