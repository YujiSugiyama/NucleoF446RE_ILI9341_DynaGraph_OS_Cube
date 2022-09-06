#ifndef __ESP32_HPP__
#define __ESP32_HPP__

// Definitions
#define SOCKET_PORT		"54000"
#define HOSTNAME			"STM32F466RE-Nucleo-ESP32"

#define SENDBUFFERLEN		512
#define RECVBUFFERLEN		1024   // 2048

#define MICROS_MS			1       // 1msec
#define IPD_TRY				4

// Variables
extern char recvBuffer[];
extern char _recvBuffer[];

// AT commands
int recvIPD(int32_t);
char* recvBody(int, int32_t);
bool sendATCommand(const char*, int32_t, const char*, const bool&);
bool sendATCommandOK(const char*, const int32_t, const bool&);
bool clearATBuffer(const int32_t);

// Analyzing command parameters
std::string getParam(const int, const char, const char, const std::string);
std::string getParamEx(const char*, const char, const char*);

// Responding to client
void sendResponse(const char*, const char*, const char*, const char*, int, int);
void sendMessage(const char*, const char*);

// Aggregated Commands
std::string getIp(void);
std::string getSDKVersion(void);
int getWifiState(void);
bool setupUTCTime(void);

int getHeaderContentValue(const char*, const char*);
std::string getHeaderContentStr(const char*, const char*);

#endif		// __ESP32_HPP__
