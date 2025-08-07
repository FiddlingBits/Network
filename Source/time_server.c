/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define TIME_SERVER_NUM_PENDING_CONNECTIONS (1)
#define TIME_SERVER_PORT                    ("8080")

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include "time_server.h"
#include <unistd.h>

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Start Time Server ***/
void timeServer_start(void)
{
    /*** Start Time Server ***/
    /* Variable */
    char buffer[1024], ipAddress[NI_MAXHOST], *localTimeString;
    ssize_t bufferLength, i;
    struct sockaddr_storage clientAddress;
    socklen_t clientAddressLength;
    int clientSocket, listenSocket, option;
    struct addrinfo hints, *serverAddress;
    time_t localTime;
    
    /* Set Up */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6; // IPv6
    hints.ai_flags = AI_PASSIVE; // Accept Connections On Any Local IP Address
    hints.ai_socktype = SOCK_STREAM; // TCP
    if(getaddrinfo(NULL, TIME_SERVER_PORT, &hints, &serverAddress) != 0)
    {
        perror("getaddrinfo");
        return;
    }
    
    /* Socket */
    if((listenSocket = socket(serverAddress->ai_family, serverAddress->ai_socktype, serverAddress->ai_protocol)) == -1)
    {
        perror("socket");
        return;
    }
    
    /* Option */
    option = 0;
    if(setsockopt(listenSocket, IPPROTO_IPV6, IPV6_V6ONLY, &option, sizeof(option)) == -1) // Support IPv4 Connections As Well As IPv6
    {
        perror("setsockopt");
        return;
    }
    option = 1;
    if(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) // Quicker Port Reuse
    {
        perror("setsockopt");
        return;
    }
    
    /* Bind */
    if(bind(listenSocket, serverAddress->ai_addr, serverAddress->ai_addrlen) == -1)
    {
        perror("bind");
        return;
    }
    freeaddrinfo(serverAddress);
    
    /* Listen */
    if(listen(listenSocket, TIME_SERVER_NUM_PENDING_CONNECTIONS) == -1)
    {
        perror("listen");
        return;
    }

    /* Wait For Connection */
    (void)printf("Wait For Connection... (e.g. curl http://127.0.0.1:8080, curl http://[::1]:8080)\n");
    clientAddressLength = sizeof(clientAddress);
    if((clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &clientAddressLength)) == -1)
    {
        perror("accept");
        return;
    }
    
    /* Connected */
    if(getnameinfo((struct sockaddr *)&clientAddress, clientAddressLength, ipAddress, sizeof(ipAddress), NULL, 0, NI_NUMERICHOST) != 0) // NI_NUMERICHOST Numeric Name (IP Address)
    {
        perror("getnameinfo");
        return;
    }
    (void)printf("Connected: %s\n", ipAddress);
    
    /* Receive */
    if((bufferLength = recv(clientSocket, buffer, sizeof(buffer), 0)) == -1) // Ignore Receive
    {
        perror("recv");
        return;
    }
    (void)printf("Rx: ");
    for(i = 0; i < bufferLength; i++)
        (void)printf("%02X", buffer[i]);
    (void)printf("\n");
    
    /* Time */
    if((localTime = time(NULL)) == -1)
    {
        perror("time");
        return;
    }
    if((localTimeString = ctime(&localTime)) == NULL)
    {
        perror("ctime");
        return;
    }
    
    /* Send */
    (void)snprintf(
        buffer, sizeof(buffer), 
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Local Time: %s\r\n", localTimeString
    );
    if((bufferLength = send(clientSocket, buffer, strlen(buffer), 0)) == -1)
    {
        perror("send");
        return;
    }
    (void)printf("Tx: ");
    for(i = 0; i < bufferLength; i++)
        (void)printf("%02X", buffer[i]);
    (void)printf("\n");
    
    /* Clean Up */
    (void)printf("Disconnect\n");
    (void)close(clientSocket);
    (void)close(listenSocket);
}
