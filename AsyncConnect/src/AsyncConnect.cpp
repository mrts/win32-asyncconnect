#include "stdafx.h"

#pragma comment(lib, "wsock32.lib")

#include "AsyncConnect.h"

#include <utilcpp/declarations.h>

#include <stdexcept>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// WinsockInitializer (from libjingle win32socketinit.cc)
///////////////////////////////////////////////////////////////////////////////

class WinsockInitializer
{
	UTILCPP_DISABLE_COPY(WinsockInitializer)

public:
	static WinsockInitializer& instance()
	{
		static WinsockInitializer singleton;
		return singleton;
	}

	~WinsockInitializer()
	{
		if (!_error)
			WSACleanup();
	}

	bool failed()
	{ return _error != 0; }

private:
	WinsockInitializer()
	{
		WSADATA wsaData;
		_error = WSAStartup(MAKEWORD(1, 1), &wsaData);
	}

	int _error;
};

///////////////////////////////////////////////////////////////////////////////
// Main window process callback for the message-only window
///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	std::cout << " <msg:" << hwnd << ":" << message
			  << ":" << wParam << ":" << lParam << "> ";

	return DefWindowProc(hwnd, message, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// WindowClassRegistrar
///////////////////////////////////////////////////////////////////////////////

const wchar_t* const CLASS_NAME = _T(__FILE__ ) _T(":class");

class WindowClassRegistrar
{
	UTILCPP_DISABLE_COPY(WindowClassRegistrar)

public:
	static WindowClassRegistrar& instance()
	{
		static WindowClassRegistrar singleton;
		return singleton;
	}

	bool failed()
	{ return _atom == 0; }

private:
	WindowClassRegistrar()
	{
		WNDCLASS windowClass = { 0, };
		windowClass.lpfnWndProc = DefWindowProc; // MessageHandler;
		windowClass.lpszClassName = CLASS_NAME;

		_atom = RegisterClass(&windowClass);
	}

	ATOM _atom;
};

///////////////////////////////////////////////////////////////////////////////
// TimerGuard
///////////////////////////////////////////////////////////////////////////////

class TimerGuard
{
public:
	TimerGuard(HWND hwnd, unsigned int timeout) :
		_hwnd(hwnd)
	{
		if (SetTimer(hwnd, 1, timeout, NULL) == 0)
			throw std::runtime_error("SetTimer() failed");
	}

	~TimerGuard()
	{
		KillTimer(_hwnd, 1);
	}

private:
	HWND _hwnd;
};

///////////////////////////////////////////////////////////////////////////////
// AsyncConnect
///////////////////////////////////////////////////////////////////////////////

#define WM_DNSNOTIFY     (WM_USER + 37)

char AsyncConnect::_hostnameBuffer[] = {0,};

AsyncConnect::AsyncConnect(const std::string& host, const unsigned int port,
		const unsigned int timeout) :
	_host(host),
	_port(port),
	_timeout(timeout),
	_address(inet_addr(host.c_str())),
	_hwnd(0),
	_socket(INVALID_SOCKET)
{
	if (_host.empty())
		throw std::runtime_error("Empty hostname");

	if (_port < 1 || _port > 65535)
		throw std::runtime_error("Invalid port: <1 or >65535");

	if (WindowClassRegistrar::instance().failed())
		throw std::runtime_error("RegisterClass() failed");

	if (WinsockInitializer::instance().failed())
		throw std::runtime_error("WSAStartup() failed");

	_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("socket() failed");

	// Create a message-only window and start the message loop
	// http://msdn.microsoft.com/en-us/library/ms632599(VS.85).aspx#message_only
	// FIXME: discuss - could make the window into a singleton
	// like WinsockInitializer above
	_hwnd = CreateWindowEx(0, CLASS_NAME, 0, 0, 0, 0, 0, 0,
			HWND_MESSAGE, 0, 0, 0);
	if (!_hwnd)
		throw std::runtime_error("CreateWindowEx() failed");
}

AsyncConnect::~AsyncConnect()
{
	if (_socket != INVALID_SOCKET)
		closesocket(_socket);
}

bool AsyncConnect::waitForMessageOrTimeout(const u_int expected_message)
{
	DWORD start = GetTickCount();
	DWORD elapsed;

	do
	{
		TimerGuard timerGuard(_hwnd, _timeout);

		MSG msg;
		BOOL ret = GetMessage(&msg, _hwnd, 0, 0);

		if (ret < 0)
			throw std::runtime_error("GetMessage() failed");
		
		if (ret == 0 || msg.message == WM_QUIT)
		{
			// FIXME: discuss - do we really want to pass WM_QUIT on?
			PostQuitMessage(static_cast<int>(msg.wParam));
			throw std::runtime_error("Unexpected WM_QUIT");
		}

		if (msg.message == expected_message)
		{
			return true;
		}
		else if (msg.message == WM_TIMER)
		{
			return false;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
    }
	while ((elapsed = GetTickCount() - start) < _timeout);

	return false;
}

bool AsyncConnect::lookupAddress()
{
	HANDLE cancelHandle = WSAAsyncGetHostByName(_hwnd, WM_DNSNOTIFY,
				_host.c_str(), _hostnameBuffer, sizeof(_hostnameBuffer));

	if (cancelHandle == 0)
		throw std::runtime_error("WSAAsyncGetHostByName() failed");

	// We need to pump messages ourselves while waiting for WM_DNSNOTIFY
	// http://blogs.msdn.com/b/oldnewthing/archive/2006/01/26/517849.aspx
	// and Win32SocketServer::Wait()

	if (!waitForMessageOrTimeout(WM_DNSNOTIFY))
	{
		// timed out
		// TODO: should deal with the return value from WSACancelAsyncRequest
		WSACancelAsyncRequest(cancelHandle);
		return false;
	}

	// TODO: setup sockaddr and verify that we have a good address

	return true;
}

AsyncConnect::Status AsyncConnect::doesConnectSuccessfully()
{
	if (_socket == INVALID_SOCKET)
		throw std::runtime_error("Socket is already closed");

	if (_address == INADDR_NONE)
	{
		if (!lookupAddress())
			return TIMEOUT;
	}
	else
	{
		std::cout << "inet_addr was successful" << std::endl;
	}

	// CloseSocket();

	return NO;
}

/*
bool AsyncConnect::CloseSocket()
{
	return false;
}
*/
