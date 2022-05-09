# psi-cv3

## Simple REST client implementation in C++

A simple REST client which checks the location of the ISS and reports whether it is currently observable at its location and whether it is on the light or dark side of the Earth.

## Build and run

To build and run the application cmake is needed. Following commands build and run the server app.
```
cmake .
make
./psi3

## Output

```
The application prints sent and received messages + data which it is using e.g. latitude and longitude. 
```
Sunrise and sunset: 
2022-05-09T01:45:14+00:00
2022-05-09T16:46:04+00:00

Current time:
2022-05-09T21:27:20
The ISS is on the DARK side of the Earth
```
## API

The application uses these APIs to get the information about the ISS and sunset/sunrise times

ISS Now API:
http://open-notify.org/Open-Notify-API/ISS-Location-Now/

Sunrise/Sunset API:
http://open-notify.org/Open-Notify-API/ISS-Location-Now/

The application was tested (not thoroughly) by comparing the output to the live ISS map at https://spotthestation.nasa.gov/tracking_map.cfm

