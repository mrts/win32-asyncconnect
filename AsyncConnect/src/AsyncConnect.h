#pragma once

#include <utilcpp/disable_copy.h>

#include <string>

class AsyncConnect
{
	UTILCPP_DISABLE_COPY(AsyncConnect)

public:

	enum Status { YES, NO, TIMEOUT };

	AsyncConnect(const std::string& host, unsigned int port,
			unsigned int timeout);
	~AsyncConnect();

	Status doesConnectSuccessfully();

private:

	bool lookupAddress();
	bool waitForMessageOrTimeout(u_int expected_message);

	std::string _host;
	unsigned int _port;
	unsigned int _timeout;

	unsigned long _address;

	static char _hostnameBuffer[MAXGETHOSTSTRUCT];
	HWND _hwnd;
	SOCKET _socket;
};
