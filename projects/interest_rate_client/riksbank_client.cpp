//The following curl fetches interest rate data from the Swedish riksbank:
//curl -v -X GET "https://api.riksbank.se/swestr/v1/all/SWESTR?fromDate=2021-11-01" -H "Cache-Control: no-cache"
//The goal is now to use socket programming in C to call the above API, and then do some interest rate analysis in C++.


#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

//openssl header for TLS handshake with server
#include <openssl/ssl.h>

using namespace std;

vector<string> splitstr(string s,char c){
    vector<string> output;

    int start = 0;
    int end = s.find(c,start+1);
    string partial = s.substr(0, end-start-1);
    output.push_back(partial);
    
    while(start>=0 && end>=0){
        start = end;
        end = s.find(c,start+1);
        partial = s.substr(start+1, end-start-1);
        output.push_back(partial);
    }

    return output;
}


void print_vector(vector<string> v){
    cout << "\n\n\n\n\n\nVector contents:\n\n";
    for(int i=0; i<v.size(); i++){
        cout << "\n\n" << v.at(i);
    }
    cout << "\n\n";
}



int main(int argc, char* argv[]){
    cout << "\n\nInterest rate modeling!\n\n";

    //Allocate memory for SSL objects for TLS handshake with server 
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;

    ctx = SSL_CTX_new(TLS_client_method()); //create ctx-object to enable tls-handshake
    if(ctx==NULL){
        cout << "\n\nFailed setting up a SSL ctx-object\n\n";
        exit(1);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); //configure client to verify server authenticity
    if(!SSL_CTX_set_default_verify_paths(ctx)){
        cout << "\n\nFailed to use the default trusted CA store!\n\n";
        exit(1);
    }

    ssl = SSL_new(ctx); //create new ssl object for intializing tls-socket
    if(ssl==NULL){
    cout << "\n\nFailed setting up a TLS-object(SSL)\n\n";
    exit(1);
    }

    //Addrinfo struct to specify API IP-adress and client IP-adress
    struct addrinfo specs, *addr_list, *addr_candidate;

    memset(&specs, 0, sizeof(specs)); //Allocate memory for network protocol specs
    specs.ai_family = AF_INET;
    specs.ai_socktype = SOCK_STREAM;
    specs.ai_protocol = 0;

    //Declare API details
    char* host = "api.riksbank.se";
    char* port = "443";
    char* path = "/swestr/v1/all/SWESTR?fromDate=2021-11-01";

    int retaddrinfo = getaddrinfo(host, port, &specs, &addr_list); //Let kernel assign available ip-adresses

    //Loop over available addresses until connection is established
    for(addr_candidate=addr_list; addr_candidate!= NULL; addr_candidate=addr_candidate->ai_next){
        cout << "\n\nhit loop\n\n";
        int sock_fd = socket(addr_candidate->ai_family, addr_candidate->ai_socktype, addr_candidate->ai_protocol);
        cout << "\n\nsocket return: " << sock_fd << "\n\n";
        if(sock_fd>=0){
            cout << "\n\nsuccess socket!";
        }
        int connectret = connect(sock_fd, addr_candidate->ai_addr, addr_candidate->ai_addrlen);

        if(connectret!=0){
            cout << "\n\nconnect fail!\n\n";
            exit(1);
        }else{
            cout << "\n\nconnected to API server";
        }
        
        //SSL connect
        int hook_to_fd = SSL_set_fd(ssl,sock_fd); //hook C-socket to SSL TLS-object
        int SNI_info_to_server = SSL_set_tlsext_status_exts(ssl, host); //Tell server which hostname the client is targeting.
        int sslConnect = SSL_connect(ssl); //SSL-connect with handshake
        cout << "\n\nssl connect result: " << sslConnect << "\n\n";
        
        //HTTP v1.1 request
        string request =
            "GET /swestr/v1/all/SWESTR?fromDate=2021-11-01 HTTP/1.1\r\n"
            "Host: api.riksbank.se\r\n"
            "User-Agent: plain-client/0.1\r\n"
            "Accept: */*\r\n"
            "Connection: close\r\n"
            "\r\n";

        // Make HTTP-request
        SSL_write(ssl, request.data(), (int)request.size());                                                                

        char read_data_buffer[8192];
        ssize_t n_bytes;

        string container; // collect API response here
        while( (n_bytes = SSL_read(ssl, read_data_buffer, sizeof(read_data_buffer)-1)) >0){
            read_data_buffer[n_bytes]='\0'; //zero-terminate buffer, else not printable to std-out
            container.append(read_data_buffer,n_bytes);
        }


        string finaldata; //remove response headers
        string json_element; //for parsing json
        vector<string> split_json; // for parsing json

        //parsing json
        int start = container.find('{');
        int end = container.find('}');
        finaldata.append(container.substr(start+1, end-start-1));
        while(start>0 && end>0){
            start = container.find('{', end);
            end = container.find('}', start);
            if(start>0 && end >0){
                finaldata.append("\n\n\n\n");
                json_element = container.substr(start+1, end-start-1);
                split_json = splitstr(json_element, ',');
                print_vector(split_json);
                finaldata.append(json_element);
            }
        }

        //cout << "\n\n\n\n\n\nfinal output:\n\n\n\n\n\n" << finaldata << "\n\n";

        cout << flush;
    }


                                        
       
    //Code to analyze interest rate data
    

    cout << "\n\n";
}
