#include <stdio.h>
#include <cstring>
#include <string>
#include <ctime>
#include <iomanip>

#include "main.h"
#include "cmsis_os.h"
#include "esp32.hpp"
#include "http.hpp"

/*******************************************************************************************************************************************
	Network Server
*******************************************************************************************************************************************/
extern "C" int server_init(void)
{
	char sendBuffer[SENDBUFFERLEN];

	if (!sendATCommandOK("CWMODE=1", 2000, false)) {
		return false;
	}
	printf("[Server]: ESP32 in Station Mode\n");

	snprintf(sendBuffer, SENDBUFFERLEN, "CWHOSTNAME=\"%s\"", HOSTNAME);
	if (!sendATCommandOK(sendBuffer, 2000, false)) {
		return false;
	}
	printf("[Server]: Hostname set to %s\n", HOSTNAME);

	const int wifiState = getWifiState();
	if (wifiState <= 0) {
		printf("\n************************** Network setting is required **************************\n");
		printf("Connect to AP(maybe send AT+CWJAP=\"SSID\",\"Passphrase\" and ctl-Q to quit)*\n");
		printf("To set IP address : Ex. AT+CIPSTA=\"192.168.1.xx\",\"192.168.1.yy\",\"255.255.255.0\"\n");
		printf("To get IP address : Ex. AT+CIFSR\n");
		printf("Anyway you need to set network addresses by manual with AT commands\n");
		printf("*********************************************************************************\n\n");
		return false;
	} else {
		printf("[Server]: Connecting to last WiFi configuration\n");
		if(!sendATCommandOK("CWJAP", 30 * 1000, false)) {
			return false;
		}
	}

	const std::string ipAddress = getIp();
	if (ipAddress == "0.0.0.0" || ipAddress.empty()) {
		printf("[Server]: Failed to get IP address\n");
		return false;
	}
	printf("[Server]: Connected to WiFi\n");
	printf("[Server]: IP Address: '%s'\n", ipAddress.c_str());

	if (!sendATCommandOK("CIPMUX=1", 2000, false)) {
		return false;
	}
	snprintf(sendBuffer, SENDBUFFERLEN, "CIPSERVER=1,%s", SOCKET_PORT);
	if (!sendATCommandOK(sendBuffer, 2000, false)) {
		return false;
	}
	printf("[Server]: Server running on port '%s'\n", SOCKET_PORT);

	if (!sendATCommandOK("CIPSTO=180", 1000, true)) {
		printf("[Server]: Failed to set client timeout\n");
	} else {
		printf("[Server]: Client timeout set to 180s\n");
	}

	return true;
}

extern "C" void dispatch_server(void)
{
	int ipd_ready;

	printf("[Server]: Server starting\n");

	while(1) {
		ipd_ready = 0;

//    if(uart_is_readable(UART_ID) && recvIPD(250)){      // IPD received?
    if(recvIPD(250)){             // IPD received?
			ipd_ready = 1;
		}

		if(ipd_ready){		// IPD receive
			const std::string response(recvBuffer);

			std::string::size_type conn = response.find("CONNECT");
			if (conn != std::string::npos) {
				printf("[Server]: Client '%c' connected\n", response[conn - 2]);
			}

			std::string::size_type cls = response.find("CLOSED");
			if (cls != std::string::npos) {
				printf("[Server]: Client '%c' disconnected\n", response[cls - 2]);
			}

			const char* ipd = strstr(recvBuffer, "+IPD");
			if (ipd != NULL) {
				printf("---------------------------------------------------------------------\n");
				printf("[IPD]: %s", ipd);
				const std::string sock = getParam(1, ',', '\0', ipd);		// cut "+IPD,"
				const std::string len = getParam(2, ',', ':', ipd);			// cut "id,"
				std::string line = getParam(2, ',', '\r', ipd);				  // get "len:........\r" thus HEADR
				line = getParam(1, ':', '\r', line);
//		  printf("Request = %s", (line+"\n").c_str());

				const std::string srv = getParam(0, ' ', '\0', line);
				int alive;
				if(srv == "GET" || srv == "POST"){
					if(getHeaderContentStr(recvBuffer, "Connection: ") == "close")
						alive = 0;
					else
						alive = 1;
				}

				if(srv == "GET"){
					http_server(sock, line, NULL, alive);
				} else if(srv == "POST"){
					int blen = getHeaderContentValue(recvBuffer,"Content-Length: ");
					char* body = recvBody(blen, 1000);
					if(body != NULL){
					  printf("Body = %s\n", body);
					  http_server(sock, line, body, alive);
				  }
				}
			}
		}
	}
}



