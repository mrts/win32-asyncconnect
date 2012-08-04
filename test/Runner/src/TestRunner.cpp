// TestRunner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "AsyncConnect\src\AsyncConnectTests.h"

#include <testcpp/StdOutView.h>

int _tmain(int, _TCHAR*)
{
	Test::Controller &c = Test::Controller::instance();
	c.setObserver(Test::observer_transferable_ptr(new Test::ColoredStdOutView));

	c.addTestSuite("Asynchronous connection tests",
			Test::Suite::instance<AsyncConnectTests>);

	int numErrors = c.run();

	std::cout << "Press ENTER to exit" << std::endl;
	std::cin.get();

	return numErrors;
}

