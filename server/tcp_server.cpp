/*!
 * \project 
 *          mini-project
 * \file 
 *          tcp_server.cpp
 * \brief 
 *          An implementation follow tcp_server.h
 */

#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>

#include "tcp_server.h"

namespace tcp_server {

  const int MAX_EVENTS = 16;
  const int MAX_BUF = 2048;
  const int QUEUE_SIZE = 5;

  int serv_fd, new_sock_fd, epoll_fd;
  struct sockaddr_in serv_addr, cli_addr;
  struct epoll_event ev, events[MAX_EVENTS];
  socklen_t clilen = sizeof(cli_addr);

  char buffer[MAX_BUF];
  std::string data_in, data_out;

  void error(const char *msg) {
    std::string tag = "TCP SERVER: ERROR ";
    tag = tag + msg;
    perror(tag.c_str());
  }

  /**
   * \brief This function establish a tcp server with event-based file descriptors
   *        control (e.g epoll_create1, epoll_ctl, epoll_wait system calls)
   * \param port
   *        The port server listen on
   * \param app
   *        An application receive a string input and return a string output respectively
   *        (e.g a http server)
   * \return int
   *         ERROR(-1)
   *         OK(0)
   *
   */
  int tcp_listen(const int port, int (*app)(const std::string&, std::string&)) {
    // setup server address properties 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
   
    // server socket
    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd == -1) {
      error("server socket");
      return -1;
    }

    // bind
    if ( bind(serv_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1 ) {
      error("binding");
      return -1;     
    } 
    
    // listen
    listen(serv_fd, QUEUE_SIZE);

    // epoll create
    epoll_fd = epoll_create1(0);
    if ( epoll_fd == -1 ) {
      error("epoll create");
      return -1;
    }
   
    // event for server file descriptor
    ev.events = EPOLLIN;
    ev.data.fd = serv_fd;
   
    // add serv_fd's event to epoll
    if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serv_fd, &ev) == -1 ) {
      error("add server event to epoll");
      return -1;
    }

    // event num
    int nfds;
    // i/o num
    int n_r;
    
    printf("tcp server is listening\n"); 
    while(1) {
      nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
        error("epoll wait");
        return -1;
      }
      for (int i = 0; i < nfds; i++) {
        if (events[i].data.fd == serv_fd) { // server file descriptor event
          new_sock_fd = accept(serv_fd, (struct sockaddr*) &cli_addr, &clilen);
          if (new_sock_fd == -1) {
            error("accept");
            return -1;
          }
          ev.events = EPOLLIN | EPOLLET;
          ev.data.fd = new_sock_fd;
          if ( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_sock_fd, &ev) == -1 ) {
            error("add client event to opoll");
            return -1;
          }
        }
        else { // client file descriptor event
          n_r = read(events[i].data.fd, buffer, MAX_BUF);
          if (n_r == -1) {
            error("read data from client");
            return -1;
          }

          if (n_r >= MAX_BUF) {
            error("request too long");
            close(events[i].data.fd);
            continue;
          }
          buffer[n_r] = 0;

	  n_r = app(buffer, data_out);
          if (n_r == -1) {
            error("middleware");
            return -1;
          }
          const char *res = data_out.c_str();
          n_r = data_out.length();
          n_r = write(events[i].data.fd, res, n_r);
          if (n_r == 1) {
            error("write data to client");
            return -1;
          }
          // communication end 
          close(events[i].data.fd);
        }
      }
    }
  } 
}

