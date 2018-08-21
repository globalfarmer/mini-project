#include <stdio.h>
#include <string>

#include "http.h"

int cons = 0;

void error(const char* msg) {
  std::string tag = "MAIN: ERROR ";
  tag = tag + msg;
  perror( tag.c_str() );
  exit(1);
}

/**
 * \brief A simple HTTP application 
 *        just echo back the with request body
 * \param req
 *        A HTTP request
 * \param res
 *        The HTTP response will store in here as output
 * \return int
 *         ERROR(-1)
 *         OK(0)
 */
int http_app(const http::http_request& req, http::http_response& res) {
  printf("connection number %d\n", ++cons);
  if (req.body.length() == 0)
    res.body = "<empty body>";
  else
    res.body = req.body;
  if (req.method == http::GET) {
    res.status = "200";
    res.reason_phrase = "OK";
    res.body = "echo GET request: \n" + res.body;
  } else if (req.method == http::POST) {
    res.status = "200";
    res.reason_phrase = "OK";
    res.body = "echo POST request: \n" + res.body;
  } else {
    res.status = "501";
    res.reason_phrase = "Not Implemented";
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    error("no port provided");
  
  int portno = atoi(argv[1]);
  printf("port = %d\n", portno);

  int ret = http_listen(portno, http_app);
    if (ret == -1)
      error("http server");
  
  return 0;
} 

