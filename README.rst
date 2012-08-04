Connecting sockets asynchronously with timeout support in Windows
=================================================================

This small example demonstrates how to connect sockets asynchronously so that
timeout is supported both during host name lookups and during actual socket
connection creation.

Note that an invisible window is needed to support messaging with the
asynchronous functions. Both `WSAAsyncGetHostByName`_ and `WSAAsyncSelect`_
require the window handle and use its message loop for sending messages to
callers.

It is inspired by `libjingle win32socketserver`_ and `licenced under the new
BSD licence`_.

**Status**: work in progress/unstable.

**Be sure to pull in submodules** with ``git clone --recursive`` when you clone.

.. _`WSAAsyncGetHostByName`: http://msdn.microsoft.com/en-us/library/windows/desktop/ms741522(v=vs.85).aspx 
.. _`WSAAsyncSelect`: http://msdn.microsoft.com/en-us/library/windows/desktop/ms741540(v=vs.85).aspx
.. _`libjingle win32socketserver`: http://code.google.com/p/libjingle/source/browse/trunk/talk/base/win32socketserver.cc
.. _`licenced under the new BSD licence`: https://github.com/mrts/win32-asyncconnect/blob/master/LICENCE.rst
