#include <stdio.h>
#include <sys/socket.h> /* SOCKET */
#include <netdb.h> /* struct addrinfo */
#include <stdlib.h> /* exit() */
#include <string.h> /* memset() and string manipulation*/
#include <errno.h> /* errno */
#include <unistd.h> /* close() */
#include <arpa/inet.h> /* IP Conversion */
#include <stdarg.h> /* va_list */

#define PROTOCOL "80"
#define MAXDATASIZE 1024*1024

void *get_in_addr(struct sockaddr *sa);
void usage();

int main (int argc, char * argv[]) {
   
    int status, i;
    char *str;
    char *domainW; //Domain with www
    char *domain; //Domain without www
    char *host;

    FILE *fp;

    if(argc <=1)
    {
        usage();
        exit(1);
    }

    str = (char *) malloc (sizeof(argv[1])); 
    strcpy(str, argv[1]); //str contain the address http://...
    

    domainW = (char *) malloc (sizeof(str)); 
    domain = (char *) malloc (sizeof(str)); 
    host = (char *) malloc (sizeof(str)); 




    //remove http://
    for(i=0; i<strlen(str)-7; i++)
    {
        domainW[i] = str[i+7];
    }

    //remove http://www.
    for(i=0; i<strlen(str)-11; i++)
    {
        domain[i] = str[i+11];
    }



    /*
    struct addrinfo {
    int              ai_flags; //use
    int              ai_family; //use
    int              ai_socktype; //use
    int              ai_protocol;
    socklen_t        ai_addrlen; //use
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};*/


    // GET ADDRESS INFO
    struct addrinfo *infos; 
    struct addrinfo hints;

    // fill hints
    memset(&hints, 0, sizeof(hints));
    //how does work memset?
    //set the first sizeof(hints) blocks it points of "&hints" to the value 0

    hints.ai_socktype = SOCK_STREAM; //SOCK_STREAM -> type of service within the family TCP (Virtual Circuit)
    hints.ai_flags = AI_PASSIVE; //set the flag to ai_passive. This can be also NULL.
    hints.ai_family = AF_UNSPEC; //With AF_UNSPEC I can use the IPV4 or IPV6. It is possible to force it with AF_INET for IPV4 or AF_INET6 for IPV6


    // get address info
    status = getaddrinfo(domain, PROTOCOL, &hints, &infos); //This function return information on a particular hostname (such as its IP address) and load up a struct sockaddr
    //I haven't used the function gethostbyname() because this function is deprecate.

    // MAKE SOCKET
    int sockfd;

    // loop, use first valid
    struct addrinfo *p;
    for(p = infos; p != NULL; p = p->ai_next) {
        // CREATE SOCKET
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol); //this function it creates a socket. If the function it fails, it returns -1
        if(sockfd == -1)
            continue;

        // TRY TO CONNECT

        status = connect(sockfd, p->ai_addr, p->ai_addrlen); //This function establishes a connection with a server TCP
        if(status == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if(p == NULL) {
        fprintf(stderr, "Failed to connect\n");
        return 1;
    }

    // LET USER KNOW
    char printableIP[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), printableIP, sizeof(printableIP)); //The inet_ntop() function shall convert a numeric address into a text string suitable for presentation. It also work with IPV6 addresses.
   

    // GET RID OF INFOS
    freeaddrinfo(infos);  //You need to call freeaddrinfo() to free up the memory

    //SEND DATA
    int s;
    char message1[1000]= "GET ";
    char message2[]= " HTTP/1.1\r\nhost: ";
    char message3[] = "\r\n\r\n";

    //Normal string. http://www...
    strcat(message1, argv[1]);
    strcat(message1, message2);
    strcat(message1, domainW);
    strcat(message1, message3);

    


    s = send(sockfd,message1, strlen(message1), 0);

    // RECEIVE DATA
    ssize_t receivedBytes;
    char buf[MAXDATASIZE];
    char *d;

    receivedBytes = recv(sockfd, buf, MAXDATASIZE-1, 0);

    if(receivedBytes == -1)

    // null terminate
    buf[receivedBytes] = '\0';

    // PRINT
    d = strchr(buf, '<'); //This function remove the useless data before the character '<'

    //Saving on file.
    fp = fopen("index.html", "w+");
    if(fp==NULL)
    {
        fprintf(stderr, "Failed open file\n");
        exit(1);
    }
    else
    {
        fprintf(fp, "%s", d);
        fclose(fp);
    }


    // CLOSE
    close(sockfd);

    return 0;
}

void *get_in_addr(struct sockaddr *sa) {
    // IP4
    if(sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *) sa)->sin_addr);

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

void usage()
{
    printf("\nUse: downloader [addressHTTP]\n");
    printf("You will have a file named 'index.html' that will contain the source of page.\n\n");
    return;
}

