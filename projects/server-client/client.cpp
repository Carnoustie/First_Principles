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
    struct addrinfo specs, *addr_list, *candidate_adress;
    
    cout << "hit";
    char *port = argv[1];


    memset(&specs, 0, sizeof(struct addrinfo));

    cout << "hit";
    specs.ai_family = AF_INET;
    specs.ai_socktype = SOCK_STREAM;
    specs.ai_protocol = 0;

    cout << "hit";
    int gai_return = getaddrinfo(NULL, port, &specs, &addr_list);
    
    cout << "gai_return: " << gai_return << "\n\n";

    for(candidate_adress=addr_list; candidate_adress!=NULL; candidate_adress=candidate_adress->ai_next){
        int sock_fd = socket(candidate_adress->ai_family, candidate_adress->ai_socktype, candidate_adress->ai_protocol);
        int bindResult = bind(sock_fd, candidate_adress->ai_addr, candidate_adress->ai_addrlen);
        cout << "\n\nbindres:" << bindResult;
        if(bindResult==0){
            int connectresult = connect(sock_fd, candidate_adress->ai_addr, candidate_adress->ai_addrlen);
            cout << "\n\nconnectresult: " << connectresult << "\n\n";
        }
    }

//    int sock_fd = socket(, int __type, int __protocol);
}
