#pragma once

#include <utilcpp/disable_copy.h>

#include <string>

class AsyncConnect
{
	UTILCPP_DISABLE_COPY(AsyncConnect)

public:

	AsyncConnect(const std::string& host, unsigned int port,
			unsigned int timeout) :
	  _host(host), _port(port), _timeout(timeout)
	{ }

	~AsyncConnect()
	{ }

	bool doesConnectSuccessfully()
	{
		return false;
	}

private:

	std::string _host;
	unsigned int _port;
	unsigned int _timeout;
};
