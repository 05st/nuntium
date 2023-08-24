#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <pthread.h>

typedef struct {
    int id;
    int socket;
    pthread_t thread;
} client_t;

int port;
int server_socket;

int client_count = 0;
client_t clients[256];

char connect_msg[64] = "Successfully reached server\n";

void init_socket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    printf("Server socket created and bound to 0.0.0.0:%d\n", port);
}

void* handle_client(void* arg) {
    client_t* client_ptr = (client_t*)arg;
    printf("Handling client %d\n", client_ptr->id);

    pthread_exit(NULL);
}

void* accept_connections() {
    listen(server_socket, 2);
    printf("%s", "Listening for connections\n");

    while (true) {
        int client_socket = accept(server_socket, NULL, NULL);

        clients[client_count].id = client_count;
        clients[client_count].socket = client_socket;

        printf("Accepted client %d connection\n", client_count);
        send(clients[client_count].socket, connect_msg, sizeof(connect_msg), 0);

        pthread_create(&clients[client_count].thread, NULL, handle_client, &clients[client_count]);
        client_count++;
    }

    pthread_exit(NULL);
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

    pthread_t t_id;
    pthread_create(&t_id, NULL, accept_connections, NULL);

    while (true) {
        int input;
        scanf("%d", &input);

        bool stop = false;        
        switch (input) {
            case 0:
            stop = true;
            break;
        }

        if (stop)
            break;
    }

    pthread_cancel(t_id);
    for (int i = 0; i < client_count; i++) {
        client_t client = clients[i];
        close(client.socket);
        pthread_cancel(client.thread);
    }
    close(server_socket);

    return 0;
}