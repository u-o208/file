#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXPENDING 5
#define RCVBUFSIZE 256
#define MAX_SIZE 256

void sendFile(int clientSocket, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    char buffer[RCVBUFSIZE];
    ssize_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (write(clientSocket, buffer, bytesRead) < 0) {
            perror("write");
            exit(1);
        }
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    int s, ns;
    unsigned short port;
    struct sockaddr_in sin, csin;
    socklen_t csinlen;

    /* 1.ソケットを作る。 */
    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    /* 2.要求待ちポート番号を決める。 */
    port = 12345;

    /* 3.待受用ポートの情報を構造体に書き込む */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    /* 4.bind()する */
    if (bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("bind");
        exit(1);
    }

    /* 5.listen()する */
    if (listen(s, MAXPENDING) < 0) {
        perror("listen");
        exit(1);
    }

    /* 6.要求が来るまでブロックする。 */
    for (;;) {
        csinlen = sizeof(csin);

        if ((ns = accept(s, (struct sockaddr *) &csin, &csinlen)) < 0) {
            perror("accept");
            exit(1);
        }

        printf("Handling client %s\n", inet_ntoa(csin.sin_addr));

        /* 7. データの読み書き */

        // ファイル名の受信
        char filename[MAX_SIZE];
        ssize_t bytesRead = read(ns, filename, sizeof(filename));
        if (bytesRead < 0) {
            perror("read");
            close(ns);
            continue;
        }

        // ファイル送信
        sendFile(ns, filename);

        /* 8. 接続の終了 */
        close(ns);
    }

    /* この部分には到達しない */
}
