/* The port number is passed as an argument */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error( const char *msg )
{
    perror(msg);
    exit(1);
}

int* xxxx()
{
    int *x = new int[10];
//    X = x;
    return x;
}

int foo11()
{
    int *x = new int[100];
    x[1] = rand();
    return x[1];
}


int main( int argc, char *argv[] )
{
    int r = *xxxx();
//    delete X;

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr{}, cli_addr{};

    // create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 1111;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
        error("ERROR on binding");

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    send(newsockfd, "Hello, world!\n", 13, 0);

    bzero(buffer, 256);

    read(newsockfd, buffer, 255);
    printf("Here is the message: %s\n", buffer);

    // Утечка ресурсов
//    close(newsockfd);
//    close(sockfd);
    return 0;
}