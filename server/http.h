/*! 
 * \project  
 *           mini-project
 * \filename 
 *           http.h
 * \brief    
 *           A http server header
 */

#include <vector>
#include <string>

namespace http {
   /**
    * \brief 
    *        enum of HTTP methods 
    *        (note: this enum is still lack 
    *        of http/1.1 supported methods)
    */ 
   typedef enum http_method {
     GET,
     POST,
     UNSUPPORT
   } http_method;

   /**
    * \brief 
    *        HTTP header struct
    */
   typedef struct http_header {
     std::string value;
   } http_header; 

   /**
    * \brief 
    *        HTTP request struct
    */
   typedef struct http_request {
     http_method method;
     std::string url;
     std::vector<http_header> headers;
     std::string body;
   } http_request; 

   /**
    * \brief
    *        HTTP response struct
    */
   typedef struct http_response {
     std::string status;
     std::string reason_phrase;
     std::vector<http_header> headers;
     std::string body; 
   } http_response;

  /**
   * \brief Establish a http server on <port> 
   *        with an <app> as a HTTP app
   * \param port 
   *        The port http server listen on
   * \param app
   *        An app that can handle a http request
   *        then return a http_response respectively
   * \return int
   *         Error(-1)
   *         Ok(0)
   */
  int http_listen(const int port, int (*app)(const http_request&, http_response&)); 

  /**
   * \brief This function will return a string presentation
   *        of a http_reponse struct input
   * \param http_response
   *        The http_response need to stringify
   * \return string
   *         The string presentation of the http response input
   */
  std::string str_from_http_response(const http_response&);
 
  /**
   * \brief This function parse string input and return
   *        the http reqeust respectively, if not return -1
   *        as sign of error
   * \return int
   *         ERROR(-1)
   *         OK(0)
   */
  int http_parse(http_request& req, const std::string &data); 
}

