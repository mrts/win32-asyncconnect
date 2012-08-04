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
		AsyncConnect google("google.com", 80, 1000);

		Test::assertTrue("Connecting to google.com works",
			google.doesConnectSuccessfully());
	}
};
