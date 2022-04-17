# psi-cv2

## Simple HTTP server implementation in C++

A simple multithreaded HTTP server which can respond to HTTP GET requests. Each request is served inside a separate thread, where it is checked whether the request is correct and the right response is selected.
The server responds to requests on:
```
/
/test
/rest
```

## Build and run

To build and run the application cmake is needed. Following commands build and run the server app.
```
cmake .
make
./psi2
```
The server runs on the local loopback address and port 1234. The port can be changed by changing the
```
#define PORT 1234
```
in main.cpp
