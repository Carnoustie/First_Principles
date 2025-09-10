//The following curl fetches interest rate data from the Swedish riksbank:
//curl -v -X GET "https://api.riksbank.se/swestr/v1/all/SWESTR?fromDate=2021-11-01" -H "Cache-Control: no-cache"
//The goal is now to use socket programming in C to call the above API, and then do some interest rate analysis in C++.


#include <iostream>
#include <openssl/ossl_typ.h>
#include <openssl/tls1.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>


//openssl headers for tls handshake with server
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace std;

int main(int argc, char* argv[]){
    cout << "\n\nInterest rate modeling!\n\n";

    //Code to fetch interest rate data
    
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    BIO* bio = NULL;

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

    //Client here
    string riksbank_ip_adress = "4.209.28.116"; //fetched by executing "nslookup api.riksbank.se" from terminal
    //string port = ???;
    //

    
    struct addrinfo specs, *addr_list, *addr_candidate;
    
    memset(&specs, 0, sizeof(specs));
    
    specs.ai_family = AF_INET;
    specs.ai_socktype = SOCK_STREAM;
    specs.ai_protocol = 0;

    char* host = "api.riksbank.se";
    char* port = "443";
    char* path = "/swestr/v1/all/SWESTR?fromDate=2021-11-01";

    int retaddrinfo = getaddrinfo(host, port, &specs, &addr_list);

    for(addr_candidate=addr_list; addr_candidate!= NULL; addr_candidate=addr_candidate->ai_next){
        cout << "\n\nhit loop\n\n";
        int sock_fd = socket(addr_candidate->ai_family, addr_candidate->ai_socktype, addr_candidate->ai_protocol);
        cout << "\n\nsocket return: " << sock_fd << "\n\n";
        if(sock_fd>=0){
            cout << "\n\nsuccess socket!";
        }
        int connectret = connect(sock_fd, addr_candidate->ai_addr, addr_candidate->ai_addrlen);

        cout << "\n\nconnect return: " << connectret << "\n\n";
        


        if(connectret==0){
            cout << "\n\nsuccess connect!\n\n";
        }

        string request = "GET /swestr/v1/all/SWESTR?fromDate=2021-11-01 HTTP/1.1\r\n";
        
        request =
            "GET /swestr/v1/all/SWESTR?fromDate=2021-11-01 HTTP/1.1\r\n"
            "Host: api.riksbank.se\r\n"
            "User-Agent: plain-client/0.1\r\n"
            "Accept: */*\r\n"
            "Connection: close\r\n"
            "\r\n";

        int hook_to_fd = SSL_set_fd(ssl,sock_fd);
        int SNI_info_to_server = SSL_set_tlsext_status_exts(ssl, host); //Tell server which hostname the client is targeting.
        
        int sslConnect = SSL_connect(ssl);

        cout << "\n\nssl connect attempt: " << sslConnect << "\n\n";
       
        // Make HTTP-request
        SSL_write(ssl, request.data(), (int)request.size());                                                                
        //write(sock_fd, request.c_str(), request.size());

        char read_data_buffer[8192];
        ssize_t n_bytes;

        string container;

        while( (n_bytes = SSL_read(ssl, read_data_buffer, sizeof(read_data_buffer)-1)) >0){
            read_data_buffer[n_bytes]='\0'; //zero-terminate buffer, else not printable to std-out
            container.append(read_data_buffer,n_bytes);
            //cout.write(read_data_buffer,n_bytes);
            //cout << "hit loop";
        }

        //cout << "output: " << container;

        string finaldata;

        int start = container.find('{');
        int end = container.find('}');
        finaldata.append(container.substr(start+1, end-start-1));
        while(start>0 && end>0){
            start = container.find('{', end);
            end = container.find('}', start);
            if(start>0 && end >0){
                finaldata.append("\n\n\n\n");
                finaldata.append(container.substr(start+1, end-start-1));
            }
        }

        cout << "\n\n\n\n\n\nfinal output:\n\n\n\n\n\n" << finaldata << "\n\n";
        cout << flush;
    }


                                        
       
    //Code to analyze interest rate data
    

    cout << "\n\n";
}
