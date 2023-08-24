#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>

typedef struct {
    int client_idx;
    int pollfd_idx;
    int socket;
} client_t;

int port;
int server_socket;

int client_count = 0;
client_t clients[256];

int pollfd_count = 0;
struct pollfd pollfds[256];

char connect_msg[64] = "Successfully reached server\n";
char buffer[256];

void init_socket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    printf("Server socket created and bound to 0.0.0.0:%d\n", port);

    listen(server_socket, 2);
    printf("%s", "Listening for connections\n");
}

void init_pollfds() {
    // stdin
    pollfds[0].fd = STDIN_FILENO;
    pollfds[0].events = POLLIN;

    // server
    pollfds[1].fd = server_socket;
    pollfds[1].events = POLLIN;

    pollfd_count = 2;
}

void cleanup() {
    for (int i = 0; i < client_count; i++)
        close(clients[i].socket);
    close(server_socket);
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

    port = atoi(argv[1]);

    printf("%s", "Nuntium Server\n");
    init_socket();
    init_pollfds();

    bool running = true;
    while (running) {
        int events = poll(pollfds, pollfd_count, 100);

        if (events == 0)
            continue;
        
        // stdin
        if (pollfds[0].revents & POLLIN) {
            int input;
            scanf("%d", &input);

            switch (input) {
                case 0:
                running = false;
                break;
            }
        }

        // server (new connection)
        if (pollfds[1].revents & POLLIN) {
            client_t client;
            client.client_idx = client_count;
            client.pollfd_idx = pollfd_count;
            client.socket = accept(server_socket, NULL, NULL);

            printf("Accepted client %d connection\n", client_count);
            send(client.socket, connect_msg, sizeof(connect_msg), 0);

            struct pollfd conn;
            conn.fd = client.socket;
            conn.events = POLLIN;

            clients[client_count++] = client;
            pollfds[pollfd_count++] = conn;
        }

        // clients
        for (int i = 0; i < client_count; i++) {
            client_t client = clients[i];
            int revents = pollfds[client.pollfd_idx].revents;
            
            if (revents & POLLIN) {
                recv(client.socket, buffer, sizeof(buffer), 0);
                printf("%d: %s", i, buffer);
            }

            if (revents & (POLLERR | POLLHUP)) {
                close(client.socket);
                printf("Client %d disconnected\n", i);
            }
        }
    }

    cleanup();

    return 0;
}