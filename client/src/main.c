#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    printf("%s", "Nuntium Client\nEnter Port: ");

    int port;
    scanf("%d", &port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

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

    while (true) {
        printf("%s", "Enter Message:\n");
        fgets(buf, 256, stdin);
        send(sock, buf, sizeof(buf), 0);
    }

    return 0;
}