/**! 
 * \project 
 *          mini project
 * \file    
 *          tcp_server.h
 * \brief   
 *          A tcp server header 
 */

#include <string>

namespace tcp_server {
  /**
   * \brief Establish a tcp server listen 
   *        on <port> with <app> as a middleware
   * \param port 
   *        The port server listen on
   * \param app
   *        The middleware receive a string input
   *        then return a string output respectively 
   * \return int
   *         Error(-1)
   *         OK(0)
   */  
  int tcp_listen(const int port, int (*app)(const std::string&, std::string&));
}

