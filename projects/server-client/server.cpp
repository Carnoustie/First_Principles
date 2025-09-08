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


    char* ipaddress = argv[1]; // server ip-adress
    char* port = argv[2]; // server port on which it listens

    

    cout << "\n\n\nThe server will run on host: " << argv[1] << ", and transmit from port " << argv[2] << "\n\n\nFull service enpoint:    "<< argv[1] << ":" << argv[2] << "\n\n\n";

    struct addrinfo specs; //for setting up the specs of the socket
    struct addrinfo *adress_list;  // will receive linked list of IP-adresses by getaddrinfo    
    struct addrinfo* ip_adress_candidate; // will hold ip adress candidates to try and bind to

    int return_variable;

    memset(&specs,0,sizeof(struct addrinfo)); //clean content of socket specs before setting socket config.

    specs.ai_family = AF_INET; //IPv4
    specs.ai_socktype = SOCK_STREAM; //supports bidirectional stream communication
    specs.ai_protocol = 0; // any protocol

    return_variable = getaddrinfo(argv[1], argv[2], &specs, &adress_list); //Extract candidate tuples ip-address:port before binding.
    if(return_variable!=0){
        cout << "could not assign address to socket! " << strerror(return_variable) << "\n\n\n";
        exit(1);
    }

    char buff[5];

    struct sockaddr clientaddr; //declare client adress. Kernel populate this adress once accept succeeds.

    //iterate over candidates until a valid adress-port allocation is found
    for(ip_adress_candidate=adress_list; ip_adress_candidate!=NULL; ip_adress_candidate=ip_adress_candidate->ai_next){
        int socket_fd = socket(ip_adress_candidate->ai_family, ip_adress_candidate->ai_socktype, ip_adress_candidate->ai_protocol);
        if(bind(socket_fd, ip_adress_candidate->ai_addr, ip_adress_candidate->ai_addrlen)==0){
            cout << "\n\n\naddress found and bound to socket!\n\n\n"; //tell kernel its a socket of a server, not client
                if( (listen(socket_fd, 5) ) ==-1){
                       cout<< "listen broke " << strerror(-1);
                    }

                socklen_t sockaddr_len = sizeof(clientaddr);
                int accept_return = accept(socket_fd, &clientaddr, &sockaddr_len);
                if(accept_return<0){
                    cout << "\n\n\naccept failed!\n\n\n";
                }

                cout << "connected!!!\n\n";
                //cout << "serveradress; " << ip_adress_candidate->ai_addr;
            
                //server is listening now
                read(accept_return,buff,3);
                cout << buff << "\n\n";
        }
    }
}
