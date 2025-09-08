//programming a client here

#include <cstddef>
#include <iostream>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;


int main(int argc, char* argv[]){
    struct addrinfo specs, *addr_list, *candidate_adress; //allocate addrinfo structs
    
    char *server_adress = argv[1]; // specify server ip adress to call
    char *port = argv[2]; // specify server port to call to

    memset(&specs, 0, sizeof(struct addrinfo)); // zero out spec struct
    
    specs.ai_family = AF_INET; // IPv4
    specs.ai_socktype = SOCK_STREAM; // bidirectional data stream
    specs.ai_protocol = 0; // any protocol

    int gai_return = getaddrinfo(argv[1], argv[2], &specs, &addr_list);  // get linked list of possible adress candidates

    int sock_fd; // allocate here to enable reuse when writing data to server
                 
    //iterate over possible adresses until success
    for(candidate_adress=addr_list; candidate_adress!=NULL; candidate_adress=candidate_adress->ai_next){
        sock_fd = socket(candidate_adress->ai_family, candidate_adress->ai_socktype, candidate_adress->ai_protocol);
        //int bindResult = bind(sock_fd, candidate_adress->ai_addr, candidate_adress->ai_addrlen);
        //cout << "\n\nbindres:" << bindResult;
        int connectresult = connect(sock_fd, candidate_adress->ai_addr, candidate_adress->ai_addrlen);
        
        if(connectresult==0){
            char host[NI_MAXHOST], service[NI_MAXHOST];
            int getnamereturn = getnameinfo(candidate_adress->ai_addr, candidate_adress->ai_addrlen, host, NI_MAXHOST, service, NI_MAXHOST,NI_NUMERICHOST);
            printf("\n\nConnected to server on <ip-address>:<port> :   %s:%s\n\n", host, service);
        }
        

    }

    //write message to server
    write(sock_fd, "hej", 3);
}
