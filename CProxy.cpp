#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include <pthread.h> // threading

/*
Written by: Ethan Huang
REF: https://www.binarytides.com/socket-programming-c-linux-tutorial/
The above constitutes for majority of the structure that I followed.
Additional references are posted inbetween the code.
*/

void *connection_handler(void *);

#define PORT     2245
#define MAXLINE 1024

int randomCoin() {
    return(rand() % 2 + 1);
}

// Driver code
int main() {
    // Address Initialization
    int socket_desc, new_socket, c, *new_sock;
    struct sockaddr_in address, client;
    //const char *message1 = "Hello Client, I have received your connection, and I will now assign a Handler for you \n";

    //address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_addr.s_addr = inet_addr("136.159.5.27");
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    
    // Socket Creation
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Socket creation Failed \n");
    }

    // Binding
    int status = bind(socket_desc, (struct sockaddr *) &address, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Binding Failed \n");
        return(1);
    }
    puts("Bind Completed! \n");

    // Listening - Just setting the socket to be able to listen
    listen(socket_desc, 5);
    
    puts("Waiting for incoming Connections... \n");
    int count;
    char buffer[100];
    // Accepting part -- Blocking -- Starting to listen. 
    int mysocket2;

    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        puts("Connection Accepted \n");

        // Reply to the client -- UNUSED
        // write(new_socket, message1, strlen(message1));

        pthread_t sniffer_thread;
        new_sock = (int*)malloc(1);
        *new_sock = new_socket;

        if ( pthread_create( &sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0 ) {
            perror("Couldn't create thread \n");
            return(1);
        }

        // Join the thread, so that we don't terminate before the thread.
        puts("Handler assigned \n");
        pthread_join( sniffer_thread, NULL);
    }

   if (new_socket < 0) {
       perror("Accept Failed");
       return(1);
   }

    close(socket_desc);
    close(mysocket2);
    return 0 ;
}

void *connection_handler(void *socket_desc) {
    // Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[2000];
    char server_reply[2000];
    char parse_Host[] = "Host: ";
    char clown2_Link[] = " http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown2.png ";
    char clown_Link[] = " http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown1.png ";
    char * pch;
    char * pHost;
    char * pPath;
    char * pIP;
    char * pHostLine;
    char * pMessage;
    char * happyPointer;
    char * full_Link_Version;

    // Create a socket to pass through to server.
    int thSocket_desc;
    thSocket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (thSocket_desc == -1) {
        printf("Failed to create Thread Socket");
    }
    printf("Socket_desc completed! \n");

    // Receive a message from client
    while((read_size = recv(sock, client_message, 2000, 0)) > 0) {
        // Prints the client message
        // printf(client_message);
        
        // Gets the Host and Path via token.
        // REF: http://web.eecs.utk.edu/~bvanderz/teaching/cs140Fa09/labs/lab3/strtok.html
        pch = strtok(client_message, "\r\n");
        pPath = pch;
        pch = strtok(NULL, "\r\n");

        // Host
        pHost = pch;
        pHost = strtok(pHost, " ");
        pHost = strtok(NULL, " ");

        // If there exists a .jpg in the path, then we must replace it with a clown.
        pch = strstr(pPath, ".jpg");
        if (pch != NULL) {
            printf("JPG found! \n");
            // Tokenize it to find the Request, and the Version
            char* Request = strtok(pPath, " ");
            char* HTTPver = strtok(NULL, " ");
            HTTPver = strtok(NULL, " ");

            // Randomizes between two Clown images
            switch(randomCoin()) {
                case 1:
                    full_Link_Version = strcat(clown_Link, HTTPver);
                    break;
                default:
                    full_Link_Version = strcat(clown2_Link, HTTPver);
            }

            // Replace the link with the clown link
            pPath = strcat(Request, full_Link_Version);

            // Prints the new Path pointer
            // printf("pPath : %s \n", pPath);

            // Replace the host with the clown host
            pHost = (char*)"pages.cpsc.ucalgary.ca";

            // Prints the new Host
            // printf("pHost is : %s \n", pHost);
        }

        // Find IP.
        struct hostent *he;
        struct in_addr **addr_list;
        int i;

        if ((he = gethostbyname(pHost)) == NULL) {
           //gethostbyname failed
            herror("gethostbyname");
            free(socket_desc);
            return(0);
        }

        // Cast the h_addr_list to in_addr, since h_addr_list has the ip address in log form
        addr_list = (struct in_addr **) he->h_addr_list;

        for (i = 0; addr_list[i] != NULL; i++) {
            // Return the first one;
            pIP = inet_ntoa(*addr_list[i]);
        }
        
        // Prints the IP that the Host resolves to
        // printf("%s resolved to : %s \n", pHost, pIP);

        // Address of the socket (recipient)
        struct sockaddr_in thSocket;

        thSocket.sin_addr.s_addr = inet_addr(pIP);
        thSocket.sin_family = AF_INET;
        thSocket.sin_port = htons(80);

        int status2 = connect(thSocket_desc, (struct sockaddr*) &thSocket, sizeof(struct sockaddr_in));
        if (status2 == -1) {
            printf("connect failed!");
            return(0);
        }
        printf("CONNECTION SUCCESS \n");

        // REF:
        // https://www.geeksforgeeks.org/how-to-append-a-character-to-a-string-in-c/
        // Path Line (absolute)
        pMessage = strcat(pPath, "\r\n");

        // Host Line
        pHostLine = strcat(parse_Host, pHost);
        pHostLine = strcat(pHostLine, "\r\n\r\n");

        pMessage = strcat(pMessage, pHostLine);

        // Prints the message that we're passing to the server
        // printf(pMessage);

        // Send the message
        int count;
        count = send(thSocket_desc, pMessage, strlen(pMessage), 0);
        if (count == -1) {
            printf("Send Failed1! \n");
        }
        printf("Data Sent. \n");

        // REF: https://stackoverflow.com/questions/19017651/how-to-send-files-in-chunk-using-socket-c-c
        // Need to loop this section in order to ensure that all bytes read are sent by chunks.
        int bytesRead = 1, bytesSent;
        while(bytesRead > 0) {
            // Read the size of the buffer
            bytesRead = recv(thSocket_desc, server_reply, sizeof(server_reply), 0);

            // If the bytes read is 0 (nothing left to read), break the loop
            if (bytesRead == 0) {
                break;
            }

            // If the bytes read is -1, then break the loop and throw an error
            if (bytesRead < 0) {
                printf("recv failed \n");
                break;
            }

            // REF: https://www.cplusplus.com/reference/cstring/strstr/
            // This replaces every instance of "Happy " in server_reply to "Silly "
            // This could potentially result in a bug, if the server response contains "Happy "
            // However, It's best to assume that's not possible.
            // First, get a pointer to the first instance of "Happy" in server_reply
            happyPointer = strstr(server_reply, "Happy");
            // While this pointer isn't NULL (it can find "Happy")
            while (happyPointer != NULL) {
                // Replace the word at the pointer with "Silly."
                strncpy(happyPointer, "Silly", 5);
                // Find the next instance of "Happy" after skipping 6 characters (To remove "Happy" from the String)
                happyPointer = strstr(happyPointer+6, "Happy");
            }
            
            // Write the bytes through the socket, which leads back to the client (Requestor)
            // Size is the same amount of bytes read.
            bytesSent = write(sock, server_reply, bytesRead);

            // If the bytes sent is -1, then throw an error
            if (bytesSent < 0) {
                perror("Failed to send message");
            }
        }
        // Print server reply -- Won't be used in the future.
        // printf(server_reply);
       
        // Clear the message.
        // REF: http://www.cplusplus.com/forum/general/28649/
        memset(client_message, 0, sizeof(client_message));
        memset(server_reply, 0, sizeof(server_reply));
    }
    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    // Free the socket pointer
    close(thSocket_desc);
    free(socket_desc);

    return(0);
}
