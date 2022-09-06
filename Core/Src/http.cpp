#include <stdio.h>
#include <cstring>
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "esp32.hpp"
#include "http.hpp"

#include "../html/contents.utf8"

/*******************************************************************************************************************************************
	URL Decoder
*******************************************************************************************************************************************/
static uint8_t code2num(uint8_t ascii)
{
	if(ascii>=0x30 && ascii<=0x39){
		return((char)(ascii - 0x30));				// '0' - '9'
	} else if(ascii>=0x41 && ascii<=0x46){
		return((char)(ascii - 0x37));				// 'A' - 'F'
	} else if(ascii>=0x61 && ascii<=0x66){
		return((char)(ascii - 0x57));				// 'a' - 'f'
	} else{
		return((char)-1);
	}
}

static void uri_decode(char* dest, const char* src)
{
	uint8_t val;

	while (*src != '\0') {
		if (*src == '%') {
			src++;						// '%'
			val = code2num(*src++);		// X(hex)
			val *= 16;
			val += code2num(*src++);	// X(hex)
			*dest++ = val;
		} else if(*src == '+'){
			*dest++=' ';
			src++;
		} else{
			*dest++ = *src++;
		}
	}
	*dest = 0;
}

static void post_analyze(char *dest, char* body)
{
	dest[0] = '\0';
	char *ans = strtok(body, "&");
	while(ans != NULL){
		uri_decode(dest, ans);
		strcat(dest, "<br/>");
		dest += strlen(dest);
		ans = strtok(NULL, "&");
	}
}

/*******************************************************************************************************************************************
	HTTP methods/functions
*******************************************************************************************************************************************/
static std::string HTTP_STATUSES(const HTTP_STATUS& status)
{
	switch (status) {
		case HTTP_STATUS::UNSUPPORTED_MEDIA_TYPE:
			return "415 Unsupported Media Type";
		case HTTP_STATUS::INTERNAL_SERVER_ERROR:
			return "500 Internal Server Error";
		case HTTP_STATUS::METHOD_NOT_ALLOWED:
			return "405 Method Not Allowed";
		case HTTP_STATUS::UNAUTHORIZED:
			return "401 Unauthorized";
		case HTTP_STATUS::BAD_REQUEST:
			return "400 Bad Request";
		case HTTP_STATUS::NOT_FOUND:
			return "404 Not Found";
		case HTTP_STATUS::OK:
			return "200 OK";
		default:
			return "500 Internal Server Error";
	}
}

static void close_connection(const std::string& sock, const HTTP_STATUS& status)
{
	std::string message = HTTP_STATUSES(status);
	std::string s_status;
	std::istringstream iss_input(message);
	std::getline(iss_input, s_status, ' ');
	message.erase(0, s_status.length() + 1);

	printf(
		"[Server]: Closing connection for client '%s' with status '%s' and message '%s'\n",
		sock.c_str(),
		s_status.c_str(),
		message.c_str()
	);

	sendResponse(
		sock.c_str(),
		HTTP_STATUSES(status).c_str(),
		"text/html",
		"", 0, 0
	);
}

static void handle_get_request(const std::string& sock, const std::string& html, int alive)
{
	for(uint32_t i=0; i<sizeof(get_html)/sizeof(struct GET_HTML); i++){
//		if (!strcmp(html.c_str(), get_html[i].name)){
		if (html == get_html[i].name){
			int len;
			if(get_html[i].len == 0){
				std::string resp(get_html[i].content);
				resp = HTML_BEGIN + resp + HTML_END;
				const char *data = resp.c_str();
				len = strlen(data);
				sendResponse(sock.c_str(), HTTP_STATUSES(HTTP_STATUS::OK).c_str(), get_html[i].type, data, len, alive);
			} else{
				len = get_html[i].len;
				sendResponse(sock.c_str(), HTTP_STATUSES(HTTP_STATUS::OK).c_str(), get_html[i].type, get_html[i].content, len, alive);
			}
			return;
		}
	}
	printf("[Server]: GET Handler 'File not found'\n");
	close_connection(sock, HTTP_STATUS::NOT_FOUND);
}

static void handle_post_request(const std::string& sock, const std::string& html, char* body, int alive)
{
	char *dec = NULL;

	for(uint32_t i=0; i<sizeof(post_html)/sizeof(struct POST_HTML); i++){
		if (html == post_html[i].name){
			std::string resp(post_html[i].content);
			if(body != NULL){
				if((dec = (char*)malloc(sizeof(char) * strlen(body))) == NULL){
          printf("[Server]: POST Handler 'Unknown Error'\n");
          close_connection(sock, HTTP_STATUS::INTERNAL_SERVER_ERROR);
          return;
				}
				post_analyze(dec, body);					// do nothing to CR/LF
				resp = HTML_BEGIN + resp + "<p>" + dec +"<p>\r\n" + HTML_HOME + HTML_END;
			} else{
				resp = HTML_BEGIN + resp + "<p>" + HTML_POST_ERROR +"<p>\r\n" + HTML_HOME + HTML_END;
			}
			const char *data = resp.c_str();
			int len = strlen(data);
			sendResponse(sock.c_str(), HTTP_STATUSES(HTTP_STATUS::OK).c_str(), post_html[i].type, data, len, alive);
			free(dec);
			return;
		}
	}
	printf("[Server]: POST Handler 'File not found'\n");
	close_connection(sock, HTTP_STATUS::NOT_FOUND);
	if(dec != NULL) free(dec);
}

/*******************************************************************************************************************************************
	HTTP server
*******************************************************************************************************************************************/
void http_server(const std::string& sock, const std::string& request, char* body, int alive)
{
  const std::string method = getParam(0, ' ', '\0', request);
	const std::string html = getParam(1, ' ', '\0', request);
	const std::string version = getParam(2, ' ', '\0', request);
	printf("[Server(%s)]: Method='%s' URL='%s' Version='%s'\n", sock.c_str(), method.c_str(), html.c_str(), version.c_str());

#if 0
	bool isAuthorized = false;
	const char* authorization = strstr(request.c_str(), "Authorization");
	if (authorization != NULL) {
		const std::string auth = getParam(0, '\r', '\0', authorization);
		const std::string authType = getParam(1, ' ', '\0', auth);
		const std::string token = getParam(2, ' ', '\0', auth);

		if (authType == "Bearer" && !token.empty()) {
			isAuthorized = verify_signature(token);
		}
	}
	if (!isAuthorized) {
		close_connection(sock, HTTP_STATUS::UNAUTHORIZED);
		return;
	}
#endif

	if (strstr(method.c_str(), "POST") != NULL) {
		handle_post_request(sock, html, body, alive);
	} else if (strstr(method.c_str(), "GET") != NULL) {
		handle_get_request(sock, html, alive);
	} else {
		close_connection(sock, HTTP_STATUS::METHOD_NOT_ALLOWED);
	}
}

