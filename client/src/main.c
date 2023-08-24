#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

int sock;
void* handle_recvs() {
    char buf[256];
    while (true) {
        recv(sock, buf, sizeof(buf), 0);
        printf("%s", buf);
    }
}

int main() {
    printf("%s", "Nuntium Client\nEnter Port: ");

    int port;
    scanf("%d", &port);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int suc = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (suc < 0) {
        printf("%s", "Couldn't connect to server\n");
        exit(EXIT_FAILURE);
    }

    char buf[256];
    recv(sock, buf, sizeof(buf), 0);

    printf("%s", buf);

    fflush(stdin);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_recvs, NULL);

    while (true) {
        char msg_buf[256];
        fgets(msg_buf, sizeof(msg_buf), stdin);
        send(sock, msg_buf, sizeof(msg_buf), 0);
    }

    pthread_cancel(tid);
    close(sock);

    return 0;
}