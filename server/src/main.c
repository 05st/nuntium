#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    printf("%s", "Nuntium Server\nEnter Port: ");

    int port;
    scanf("%d", &port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    listen(sock, 1);

    int csock = accept(sock, NULL, NULL);

    char msg[128] = "Reached server\n";
    send(csock, msg, sizeof(msg), 0);
    printf("%s", "Accepted client connection\n");

    char buf[256];
    while (true) {
        recv(csock, buf, sizeof(buf), 0);
        printf("%s", buf);
    }

    return 0;
}