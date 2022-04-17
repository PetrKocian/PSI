#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>


#define BACKLOG 10
#define NUM_THREADS 10
#define BUFFER_SIZE 1024
#define PORT 1234

//check if there is a GET request in buffer, return end of first line or -1 for error
int check_request(const char *request)
{
    std::string req(request);

    //check GET method
    if(req.find("GET")!=0)
    {
        return -1;
    }
    //check if HTTP is specified
    auto http = req.find("HTTP/");
    if(http==std::string::npos)
    {
        return -1;
    }
    //check if there is a newline after HTTP
    int endl = req.find("\n", http);
    if(endl==std::string::npos)
    {
        return -1;
    }
    return endl;
}

//return response string according to address
std::string get_response(const char *request)
{
    std::cout << request << std::endl;
    std::string req(request);
    std::string retstr = "";
    //get the right position in the request
    int slash = req.find("/");
    int space = req.find(" ", 5);
    std::string substr = req.substr(slash+1, space-slash-1);

    //choose answer
    if(substr.empty()){
        retstr = "Hello world!!!";
    }
    if(substr.compare("test")==0){
        retstr = "Hello world 1";
    }
    if(substr.compare("rest")==0){
        retstr = "Hello world 2";
    }

    return retstr;
}

//read request from client socket
std::string read_request(long client_socket)
{
    char recvbuff[BUFFER_SIZE];

    read(client_socket, recvbuff, sizeof(recvbuff));

    int endl = check_request(recvbuff);

    if(endl<0){
        return "";
    }
    recvbuff[endl] = 0;
    std::string response = get_response(recvbuff);

    return response;
}

//serve client - runs in a thread
void * serve_client(void* client_sock)
{
    std::string response;
    int client_socket = (long)client_sock;

    std::string retstr = read_request(client_socket);

    if(retstr.empty()){
        response = "HTTP/1.1 " + std::to_string(404) + "\n\n";
    }
    else{
        response = "HTTP/1.1 " + std::to_string(200) + " OK\n\n"
                               + retstr +"\r\n";
    }

    write(client_socket, response.c_str(), sizeof(response));
    close(client_socket);
}

//prints address and port of the server to console
void reportAddr(struct sockaddr_in *server_addr)
{
    char hostbuff[INET6_ADDRSTRLEN];
    char servicebuff[NI_MAXSERV];

    if(auto error = getnameinfo(reinterpret_cast<const sockaddr *>(server_addr),
                   sizeof(*server_addr), hostbuff, sizeof(hostbuff),
                   servicebuff, sizeof(servicebuff), NI_NUMERICHOST) < 0){
        std::cout << "Error: "<< gai_strerror(error) << std::endl;
        return;
    }
    std::cout << "Server listening on: " <<hostbuff << " port: " <<servicebuff << std::endl;
}


int main() {
    //init server
    int server_socket;
    long client_socket;
    struct sockaddr_in server_addr{};
    int one  = 1;

    pthread_t threads[NUM_THREADS];
    int i = 0;
    int port = PORT;

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "Error creating socket" << std::endl;
        return 0;
    }

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if(bind(server_socket,(struct sockaddr *) &server_addr, sizeof (server_addr)) < 0){
        std::cout << "Error while binding" << std::endl;
        perror("");
        return 0;
    }

    if(listen(server_socket, BACKLOG) < 0){
        std::cout << "Error while trying to listen" << std::endl;
        return 0;
    }

    //print address and port
    reportAddr(&server_addr);

    //serve clients
    while(1) {
        client_socket = accept(server_socket, NULL, NULL);
        //create thread
        pthread_create(&threads[i++], NULL, serve_client, (void*)client_socket);
        //if there are already too many threads running, wait until they finish
        if( i >= NUM_THREADS)
        {
            i = 0;
            //wait for threads to finish once we reach 10, then start from 0 again
            while(i < NUM_THREADS)
            {
                std::cout << "Waiting for thread: " << i << std::endl;
                pthread_join(threads[i++],NULL);
            }
            i = 0;
        }
    }
    return 0;
}
