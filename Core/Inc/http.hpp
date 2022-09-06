#ifndef __HTTP_HPP__
#define __HPPT_HPP__

enum class HTTP_STATUS {
	UNSUPPORTED_MEDIA_TYPE,
	INTERNAL_SERVER_ERROR,
	METHOD_NOT_ALLOWED,
	UNAUTHORIZED,
	BAD_REQUEST,
	NOT_FOUND,
	OK
};

void http_server(const std::string&, const std::string&, char*, int);

#endif		// __HTTP_HPP__

