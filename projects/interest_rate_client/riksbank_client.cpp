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

using namespace std;

int main(int argc, char* argv[]){
    cout << "\n\nInterest rate modeling!\n\n";

    //Code to fetch interest rate data
    
    
    //Code to analyze interest rate data
    

    cout << "\n\n";
}
