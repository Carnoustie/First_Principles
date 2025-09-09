#include <iostream>
#include <string>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int main() {
    const char* host = "api.riksbank.se";
    const char* port = "443";
    const char* path = "/swestr/v1/all/SWESTR?fromDate=2021-11-01";

    // 1) Resolve
    addrinfo hints{}, *res=nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(host, port, &hints, &res);
    if (rc != 0) { std::cerr << "getaddrinfo: " << gai_strerror(rc) << "\n"; return 1; }

    // 2) Connect
    int fd = -1;
    for (addrinfo* p = res; p; p = p->ai_next) {
        int s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == -1) continue;
        if (connect(s, p->ai_addr, p->ai_addrlen) == 0) { fd = s; break; }
        close(s);
    }
    freeaddrinfo(res);
    if (fd == -1) { std::cerr << "connect failed\n"; return 1; }

    // 3) TLS setup
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) { std::cerr << "SSL_CTX_new failed\n"; return 1; }

    // verify system CAs
    if (!SSL_CTX_set_default_verify_paths(ctx)) {
        std::cerr << "No default CA store\n"; /* you can also set custom store */
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) { std::cerr << "SSL_new failed\n"; SSL_CTX_free(ctx); return 1; }
    SSL_set_fd(ssl, fd);
    SSL_set_tlsext_host_name(ssl, host);   // SNI

    if (SSL_connect(ssl) != 1) {
        std::cerr << "TLS handshake failed\n";
        ERR_print_errors_fp(stderr);
        SSL_free(ssl); SSL_CTX_free(ctx); close(fd);
        return 1;
    }

    // 4) Send a valid HTTP/1.1 request
    std::string req =
        std::string("GET ") + path + " HTTP/1.1\r\n"
        "Host: " + host + "\r\n"
        "User-Agent: riksbank-client/0.1\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n"
        "\r\n";

    int sent = SSL_write(ssl, req.data(), (int)req.size());
    if (sent <= 0) {
        std::cerr << "SSL_write failed\n";
        ERR_print_errors_fp(stderr);
    }

    // 5) Read and print the response
    char buf[8192];
    int n;
    while ((n = SSL_read(ssl, buf, sizeof(buf))) > 0) {
        std::cout.write(buf, n);
    }
    std::cout << std::flush;

    // 6) Cleanup
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(fd);
    EVP_cleanup(); // optional in newer OpenSSL

    return 0;
}

