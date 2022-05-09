#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tuple>
#include <cstring>
#include <iomanip>
#include <chrono>

#define ISS_IP "138.68.39.196"
#define SUN_IP "45.33.59.78"

#define PORT 80

std::tuple<std::string, std::string, int> iss_get()
{
    int socketfd;
    char reply[1024];
    memset(&reply, 0, sizeof(reply));

    std::string msg =   "GET /iss-now.json HTTP/1.0\r\n"
                        "Host: api.open-notify.org\r\n"
                        "Connection: close\r\n\r\n";


    std::cout << "Sending message:\n--------------\n";
    std::cout << msg << "--------------\n";

    auto iaddr = inet_addr(ISS_IP);

    struct sockaddr_in servaddr;

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "Error creating socket";
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = iaddr;
    int len = sizeof(sockaddr_in);

    connect(socketfd, reinterpret_cast<sockaddr*>(&servaddr), len);

    send(socketfd, msg.c_str(), msg.length(), 0);

    recv(socketfd, reply, sizeof(reply), 0);

    std::cout << "Reply:\n--------------\n" << reply << "\n--------------\n";

    //string magic
    std::string reply_s(reply);
    int pos = reply_s.find("latitude");
    std::string lat = reply_s.substr(pos+12);
    pos = lat.find("\"");
    lat = lat.substr(0,pos);

    pos = reply_s.find("longitude");
    std::string lon = reply_s.substr(pos+13);
    pos = lon.find("\"");
    lon = lon.substr(0,pos);

    pos = reply_s.find("timestamp");
    std::string timestamp_s = reply_s.substr(pos+12);
    pos = timestamp_s.find("}");
    timestamp_s = timestamp_s.substr(0,pos);
    int timestamp = std::stoi(timestamp_s);


    std::cout << "latitude, longitude, and timestamp: \n";
    std::cout << lat << std::endl;
    std::cout << lon << std::endl;
    std::cout << timestamp << std::endl << std::endl;

    return std::tuple<std::string, std::string, int>(lat,lon, timestamp);

}

std::tuple<int, int> sun_get(std::string lat, std::string lon)
{
    int socketfd;
    char reply[1024];
    memset(&reply, 0, sizeof(reply));

    std::string msg =   "GET /json?lat="+ lat +"&lng="+ lon +"&formatted=0 HTTP/1.0\r\n"
                        "Host: api.sunrise-sunset.org\r\n"
                        "Connection: close\r\n\r\n";


    std::cout << "Sending message:\n--------------\n";
    std::cout << msg << "--------------\n";

    auto iaddr = inet_addr(SUN_IP);

    struct sockaddr_in servaddr;

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "Error creating socket";
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = iaddr;
    int len = sizeof(sockaddr_in);

    connect(socketfd, reinterpret_cast<sockaddr*>(&servaddr), len);

    send(socketfd, msg.c_str(), msg.length(), 0);

    recv(socketfd, reply, sizeof(reply), 0);

    std::cout << "Reply:\n--------------\n" << reply << "\n--------------\n";

    //string magic
    std::string reply_s(reply);
    int pos = reply_s.find("sunrise");
    std::string sunrise = reply_s.substr(pos+10);
    pos = sunrise.find("\"");
    sunrise = sunrise.substr(0,pos);

    pos = reply_s.find("sunset");
    std::string sunset = reply_s.substr(pos+9);
    pos = sunset.find("\"");
    sunset = sunset.substr(0,pos);

    std::cout << "Sunrise and sunset: \n";
    std::cout << sunrise << std::endl;
    std::cout << sunset << std::endl;

    std::stringstream sunset_ss;
    sunset_ss << sunset;

    std::stringstream sunrise_ss;
    sunrise_ss << sunrise;

    //return timestamps for sunset and sunrise
    struct tm tm;
    sunrise_ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    std::time_t time_sunrise = timegm(&tm);

    sunset_ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    std::time_t time_sunset = timegm(&tm);

    return std::tuple<int, int>(time_sunrise, time_sunset);

}

int main() {
    std::string addr;
    std::cout << "Hit [enter] to start\n--------------\n";
    std::getline(std::cin, addr);
    //get iss info
    auto tup = iss_get();
    //get time info
    auto tupp = sun_get(std::get<0>(tup),std::get<1>(tup));
    int current_time = std::get<2>(tup);
    int sunrise =   std::get<0>(tupp);
    int sunset = std::get<1>(tupp);


    char curr[256];
    memset(&curr, 0, sizeof(curr));

    //convert current timestamp to readable format
    std::time_t temp = current_time;
    std::tm* t = std::gmtime(&temp);
    strftime(curr, sizeof(curr) , "%Y-%m-%dT%H:%M:%S", t);

    std::cout << "\n" << "Current time:\n" << curr << "\n";

    //Check if it's after sunset/before sunrise -> dark side, otherwise light
    if(current_time < (sunrise) || current_time > (sunset))
    {
        std::cout << "The ISS is on the DARK side of the Earth\n";
    }
    else
    {
        std::cout << "The ISS is on the LIGHT side of the Earth\n";
    }

    //Check if ISS is observable sunset/sunrise +-90 minutes
    if(current_time < (sunrise - 5400) || current_time > (sunset + 5400))
    {
        std::cout << "Space station is observable\n";
    }
    else
    {
        std::cout << "Space station is NOT observable\n";
    }
}
