#pragma once

#include "AsyncConnect.h"

#include <testcpp/testcpp.h>
#include <utilcpp/disable_copy.h>

class AsyncConnectTests : public Test::Suite
{
    UTILCPP_DISABLE_COPY(AsyncConnectTests)

public:
	AsyncConnectTests()
	{ }

	void test()
	{
		testConnectByAddress();
		testConnectByHostname();
		testConnectByAddressTimeout();
		testConnectByHostnameTimeout();
	}

	void testConnectByAddress()
	{
		AsyncConnect google("74.125.143.100", 80, 5000);

		Test::assertEqual<AsyncConnect::Status>("Connecting to 74.125.143.100 works",
			google.doesConnectSuccessfully(), AsyncConnect::YES);
	}

	void testConnectByHostname()
	{
		AsyncConnect google("google.com", 80, 5000);

		Test::assertEqual<AsyncConnect::Status>("Connecting to google.com works",
			google.doesConnectSuccessfully(), AsyncConnect::YES);
	}

	void testConnectByAddressTimeout()
	{
		AsyncConnect google("74.125.143.100", 80, 5);

		Test::assertEqual<AsyncConnect::Status>("Connecting to 74.125.143.100 times out with short timeout",
			google.doesConnectSuccessfully(), AsyncConnect::TIMEOUT);
	}

	void testConnectByHostnameTimeout()
	{
		AsyncConnect google("google.com", 80, 5);

		Test::assertEqual<AsyncConnect::Status>("Connecting to google.com times out with short timeout",
			google.doesConnectSuccessfully(), AsyncConnect::TIMEOUT);
	}

	void testConnectToInvalidAddress()
	{
		AsyncConnect nonRoutable("10.255.255.1", 80, 5000);

		Test::assertEqual<AsyncConnect::Status>("Connecting to 10.255.255.1 does not work",
			nonRoutable.doesConnectSuccessfully(), AsyncConnect::NO);
	}

	void testConnectToInvalidHostname()
	{
		AsyncConnect nonExisting("non-existing-hostname-0.8936516037300068", 80, 5000);

		Test::assertEqual<AsyncConnect::Status>("Connecting to non-existing hostname does not work",
			nonExisting.doesConnectSuccessfully(), AsyncConnect::NO);
	}

	void testConnectToInvalidPort()
	{
		AsyncConnect google("74.125.143.100", 65000, 5000);

		Test::assertEqual<AsyncConnect::Status>("Connecting to invalid port does not work",
			google.doesConnectSuccessfully(), AsyncConnect::YES);
	}

};
