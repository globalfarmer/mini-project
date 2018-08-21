#!/bin/bash

if test -z "$1"
then
  echo "No Port number was supplied"
else
  g++ --std=c++11 -c tcp_server.cpp http.cpp
  ar rvs lib.a tcp_server.o http.o
  echo "Server is listening on port $1"
  g++ --std=c++11 main.cpp lib.a -o main.out && ulimit -n 131072 && ./main.out $1 
fi

