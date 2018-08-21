/*!
 *  \project 
 *           mini-project
 *  \file    
 *           http.cpp
 *  \brief   
 *           Implemtation an HTTP server handle a HTTP request then
 *           return the reseponse respectively based on HTTP/1.1
 *
 */

#include <string>
#include <string.h>
#include <vector>

#include "tcp_server.h"
#include "http.h"

namespace http {
  int (*app)(const http_request&, http_response&);

  void error(const char* msg) {
    std::string tag = "HTTP: ERROR ";
    tag = tag + msg;
    perror(tag.c_str());
  }

  int next_line(const std::string& data_in, const int idx, std::string& line) {
    int ret;
    if ((ret = data_in.find("\r\n", idx)) == std::string::npos)
      return -1;
    line = data_in.substr(idx, ret-idx);
    return ret+2; 
  }
  
  int http_parse(http_request& req, const std::string& data_in) {
    std::size_t idx = 0, idx_tmp;
    std::size_t l_idx[3]; l_idx[0] = -1;
    std::string line, tmp;

    // request line
    if ( (idx = next_line(data_in, idx, line)) == -1 ) {
      return -1;
    } 
    for (int i = 1; i <= 2; i++) {
      l_idx[i] = line.find(' ', l_idx[i-1]+1); 
      if (l_idx[i] == std::string::npos)
        return -1; 
    }
    tmp = line.substr(0, l_idx[1]);
    req.url = line.substr(l_idx[1]+1, l_idx[2]-l_idx[1]-1);
    if (tmp == "GET")
      req.method = GET;
    else if (tmp == "POST")
      req.method = POST;
    else
      req.method = UNSUPPORT;
    // request line - version
    tmp = line.substr(l_idx[2]+1, line.length()-l_idx[2]-1);
    if (tmp != "HTTP/1.1") return -1; 
   
    // headers
    while (1) {
      if ((idx = next_line(data_in, idx, line)) == -1) {
        return -1; 
      }
      if (line.length() == 0)
        break;
      req.headers.push_back({.value = line});
    }

    // body
    req.body = data_in.substr(idx, data_in.length() - idx); 
    return 0;
  }

  /**
   * \brief A TCP aplication employ a another HTTP application for output
   *        (e.g string_in -> http_req -> app -> http_res -> string_out)
   * \param data_in
   *        Input data as a string
   * \param data_out
   *        Output will store in this param
   * \return int
   *         ERROR(-1)
   *         OK(0)
   *         
   */
  int http_main(const std::string& data_in, std::string& data_out) {
    // null header
    http_response error_res;
    error_res.reason_phrase = "xxx-xxx-xxx";
    error_res.headers = std::vector<http_header>();
   
    // parsing
    http_request req;
    if (http_parse(req, data_in) == -1) {
      error("parsing");
      error_res.status = "400";
      error_res.reason_phrase = "Bad Request";
      data_out = str_from_http_response(error_res);
      return 1;
    }
  
    // get data from app
    http_response res;
    if (app(req, res) == -1) {
      error("app");
      error_res.status = "500";
      error_res.reason_phrase = "Internal Server Error";
      data_out = str_from_http_response(error_res);
      return 1;
    }

    // return http response as a string 
    data_out = str_from_http_response(res);
    return 0;
  }
  
  /**
   * \brief This function establish a http server on <port>
   * \param port
   *        The port server listen on
   * \param _app
   *        A HTTP application
   * \return int
   *         ERROR(-1)
   *         OK(0)  
   */ 
  int http_listen(const int port, int (*_app)(const http_request&, http_response&)) {
    app = _app;
    if (tcp_server::tcp_listen(port, http_main) == -1) {
      error("tcp server");
      return -1;
    }
    return 0;
  }
  
  std::string str_from_http_response(const http_response& res) {
    // status-line
    std::string ret = "HTTP/1.1 ";
    ret = ret + res.status;
    ret = ret + " ";
    ret = ret + res.reason_phrase;
    ret = ret + "\r\n";
   
    // headers
    for (int i = 0; i < res.headers.size(); i++) {
      ret += res.headers[i].value;
      ret += "\r\n";
    }
    
    // end header
    ret += "\r\n";
   
    // body
    ret += res.body;
   
    return ret;
  }
}

