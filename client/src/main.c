#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <ncurses.h>

int port;
char* addr;

int client_socket;
struct pollfd pollfds[2];

char buffer[256];

int init_socket() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(addr);

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
    if (argc != 3) {
        if (argc < 3)
            printf("%s", "Too few arguments supplied.\n");
        if (argc > 3)
            printf("%s", "Too many arguments supplied.\n");
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    initscr();

    printw("%s", "Nuntium Client\n");
    refresh();

    port = atoi(argv[2]);
    addr = argv[1];

    if (init_socket() < 0) {
        printw("%s", "Couldn't connect to server\n");
        refresh();
        exit(EXIT_FAILURE);
    }

    init_pollfds();

    while (true) {
        int events = poll(pollfds, 2, 500);

        if (events == 0)
            continue;

        // stdin
        if (pollfds[0].revents & POLLIN) {
            getstr(buffer);
            refresh();

            if (strcmp(buffer, "/exit") == 0)
                break;
            
            sprintf(buffer, "%s\n", buffer);

            send(client_socket, buffer, sizeof(buffer), 0);
        }

        // socket
        if (pollfds[1].revents & POLLIN) {
            recv(client_socket, buffer, sizeof(buffer), 0);

            printw("%s", buffer);
            refresh();
        }
    }

    endwin();
    close(client_socket);

    return 0;
}