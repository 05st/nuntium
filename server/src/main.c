#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

typedef struct {
    struct pollfd* pollfd_ptr;
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

void relay_message(int sender) {
    for (int i = 0; i < client_count; i++) {
        if (i == sender) continue;

        client_t client = clients[i];
        send(client.socket, buffer, sizeof(buffer), 0);
    }
}

void cleanup() {
    for (int i = 0; i < client_count; i++)
        close(clients[i].socket);
    close(server_socket);
}

void handle_disconnect(int idx) {
    close(clients[idx].socket);

    for (int i = idx + 1; i < client_count; i++) {
        clients[i - 1] = clients[i];
        // since we shift the pollfds array back as well
        clients[i - 1].pollfd_ptr--;
    }
    client_count--;

    int pollfd_idx = clients[idx].pollfd_ptr - pollfds; // bit of a hack maybe

    for (int i = pollfd_idx + 1; i < pollfd_count; i++)
        pollfds[i - 1] = pollfds[i];
    pollfd_count--;

    printf("Client %d disconnected\n", idx);
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
        int events = poll(pollfds, pollfd_count, 500);

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
            client.socket = accept(server_socket, NULL, NULL);

            struct pollfd conn;
            conn.fd = client.socket;
            conn.events = POLLIN;

            pollfds[pollfd_count] = conn;

            client.pollfd_ptr = &pollfds[pollfd_count];
            clients[client_count] = client;

            printf("Accepted client %d connection\n", client_count);
            send(client.socket, connect_msg, sizeof(connect_msg), 0);

            client_count++;
            pollfd_count++;
        }

        // clients
        for (int i = 0; i < client_count; i++) {
            client_t client = clients[i];

            int revents = client.pollfd_ptr->revents;

            // client disconnected
            if (revents & (POLLERR | POLLHUP)) {
                handle_disconnect(i);
                // don't want to skip the next client since handle_disconnect shifts the arrays back
                i--;
                continue;
            }
            
            if (revents & POLLIN) {
                recv(client.socket, buffer, sizeof(buffer), 0);
                relay_message(i);
                printf("%d: %s", i, buffer);
            }
        }
    }

    cleanup();

    return 0;
}