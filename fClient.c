#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 256

int main(int argc, char *argv[])
{
        int s;
        char *servIP;
        unsigned short port;
        struct sockaddr_in sin;
        char filename[MAX_SIZE];
        char buffer[MAX_SIZE];

        /* 1.ソケットを作る。 */
        if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                perror("socket");
                exit(1);
        }

        /* 2.サーバの情報を得る */
        servIP = "127.0.0.1";
        port = 12345;
        
        /* 3.サーバの情報を構造体に書込む */
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(servIP);
        sin.sin_port = htons(port);

        /* 4.connect()する */
        if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
            perror("connect");
            exit(1);
        }
        
        /* 5.ファイル名の入力 */
        printf("ファイル名：");
        scanf("%s", filename);

        /* 6.サーバにファイル名を送信 */
        if (write(s, filename, strlen(filename) + 1) < 0) {
            perror("write");
            exit(1);
        }

        /* 7.サーバからのデータを受信し表示 */
        ssize_t bytesRead;
        while ((bytesRead = read(s, buffer, sizeof(buffer))) > 0) {
            printf("%.*s", (int)bytesRead, buffer);
        }
        if (bytesRead < 0) {
            perror("read");
            exit(1);
        }
        

        /* 8.接続終了 */
        close(s);
        
        return 0;
}
