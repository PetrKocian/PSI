#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>


#define PORT    1900
#define SSDP    "239.255.255.250"


int main() {
    int socketfd, rcv_ret;
    std::string addr;
    char reply[1024];
    std::cout << "Hit [enter] to send SSDP discover packet\n";
    std::getline(std::cin, addr);
    addr = SSDP;
    std::string msg = "M-SEARCH * HTTP/1.1\r\n"
                      "HOST: 239.255.255.250:1900\r\n"
                      "MAN: \"ssdp:discover\"\r\n"
                      "MX: 2\r\n"
                      "ST: ssdp:all\r\n\r\n";



    std::cout << "Sending message:\n\n";
    std::cout << msg;

    auto iaddr = inet_addr(addr.c_str());

    struct sockaddr_in servaddr;

    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        std::cout << "Error creating socket";
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&reply, 0, sizeof(reply));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = iaddr;
    int len = sizeof(sockaddr_in);

    sendto(socketfd, msg.c_str(), msg.length(), 0, (sockaddr *)&servaddr, len);
    std::cout << "Message sent\n\n";

    rcv_ret = recvfrom(socketfd, reply, 1024, 0, (sockaddr *)&servaddr, (socklen_t *)&len);

    if(rcv_ret  == -1)
    {
        fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
    }

    std::cout << "Receive:\n\n";
    std::cout << reply;

    return 0;
}
