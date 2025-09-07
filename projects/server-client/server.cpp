// Programming a server here

#include <iostream> //for printing to cout
#include <sys/types.h> 
#include <sys/socket.h> // c-sockets
#include <netdb.h> //for getaddrinfo
#include <string.h> // for error handling
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;


void print_sockaddr_info(struct addrinfo* sockaddr){
    cout << "\n\n\naddrinfo:\n\n\n";
    cout << "\n\n\nprotocol family:"<< sockaddr->ai_family <<"\n\n\n";
    cout << "\n\n\nprotocol:"<< sockaddr->ai_protocol <<"\n\n\n";
    cout << "\n\n\nsocktype:" << sockaddr->ai_socktype << "\n\n\n";
    // cout << "\n\n\naddress:" << sockaddr->ai_addr->sa_data << "\n\n\n";
}



int main(int argc, char* argv[]){

    cout << "\n\n\nThe server will run on host: " << argv[1] << ", and transmit from port " << argv[2] << "\n\n\nFull service enpoint:    "<< argv[1] << ":" << argv[2] << "\n\n\n";

    char* ipaddress = argv[1];
    char* port = argv[2];


    struct addrinfo specs; //for setting up the specs of the socket
    struct addrinfo *adress_list;  // will receive linked list of IP-adresses by getaddrinfo    

    int return_variable;
    struct addrinfo* ip_adress_candidate;


    memset(&specs,0,sizeof(struct addrinfo));

    specs.ai_family = AF_UNSPEC; // both IPV 4 and IPV 6
    specs.ai_socktype = SOCK_STREAM; //supports bidirectional stream communication
    specs.ai_flags = AI_PASSIVE;
    specs.ai_canonname = NULL;
    specs.ai_protocol = 0; // any protocol
    specs.ai_addr = NULL; // any protocol
    specs.ai_next = NULL; // any protocol


    print_sockaddr_info(&specs);

    // int socket_fd = socket(specs.ai_family, specs.ai_socktype, specs.ai_protocol);


    return_variable = getaddrinfo(ipaddress, port, &specs, &adress_list); //Extract candidate tuples ip-address:port before binding.
    if(return_variable!=0){
        cout << "could not assign address to socket! " << strerror(return_variable) << "\n\n\n";
        exit(1);
    }

    //iterate over candidates until a port is found

    for(ip_adress_candidate=adress_list; ip_adress_candidate!=NULL; ip_adress_candidate=ip_adress_candidate->ai_next){
        cout << "\n\n\naddress: " << ip_adress_candidate->ai_addr;
        int socket_fd = socket(ip_adress_candidate->ai_family, ip_adress_candidate->ai_socktype, ip_adress_candidate->ai_protocol);
        cout << "\n\n\nsocketfd: " << socket_fd;

        if(bind(socket_fd, ip_adress_candidate->ai_addr, ip_adress_candidate->ai_addrlen)==0){
            cout << "\n\n\naddress found and bound to socket!\n\n\n";
                if( (listen(socket_fd, 5) ) ==-1){
                       cout<< "listen broke " << strerror(-1);
                    }
                socklen_t sockaddr_len;
                int accept_return = accept(socket_fd, ip_adress_candidate->ai_addr, &sockaddr_len);
                if(accept_return<0){
                    cout << "\n\n\naccept failed!\n\n\n";
                }
        }
    }









}