#include "CommandPipe.hpp"

namespace Command::Pipe
{
	// Heavily referenced from here: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365588(v=vs.85).aspx
	// This signature is required when running a function on a thread.
	DWORD WINAPI startPipeManager(LPVOID lpParam) {

		HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
		BOOL connected = false;
		DWORD threadID = 0;

		// Pipe manager must run indefinitely.
		while (true) {

			hPipe = CreateNamedPipe(
				pipeName,             // pipe name 
				PIPE_ACCESS_DUPLEX,       // read/write access 
				PIPE_TYPE_MESSAGE |       // message type pipe 
				PIPE_READMODE_MESSAGE |   // message-read mode 
				PIPE_WAIT,                // blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				BUFFERSIZE,                  // output buffer size 
				BUFFERSIZE,                  // input buffer size 
				0,                        // client time-out 
				NULL);                    // default security attribute

			if (hPipe == INVALID_HANDLE_VALUE) {
				// Error logging functionality here.
				return -1;
			}

			connected = ConnectNamedPipe(hPipe, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

			if ( connected ) {

				hThread = CreateThread(
					NULL,              // no security attribute 
					0,                 // default stack size 
					handlePipeThread,    // thread proc
					(LPVOID) hPipe,    // thread parameter 
					0,                 // not suspended 
					&threadID);      // returns thread ID 

				if ( hThread == NULL ) {
					return -1;
				} else {
					CloseHandle(hThread);
				}

			} else {
				// Client couldn't connect, close pipe.
				CloseHandle(hThread);
			}

		}

	}

	DWORD WINAPI handlePipeThread(LPVOID lpvParam) {

		HANDLE hHeap = GetProcessHeap();
		TCHAR *pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFFERSIZE * sizeof(TCHAR));
		TCHAR *pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFFERSIZE * sizeof(TCHAR));

		DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
		BOOL success = FALSE;
		HANDLE hPipe = NULL;

		if ( lpvParam == NULL ) {
			// Error logging here
			return (DWORD) -1;
		}
		if ( pchRequest == NULL ) {
			// Error logging here
			return (DWORD) -1;
		}
		if ( pchReply == NULL ) {
			// Error logging here
			return (DWORD) -1;
		}

		hPipe = (HANDLE) lpvParam;

		while (true) {

			success = ReadFile(
				hPipe,
				pchRequest,
				BUFFERSIZE * sizeof(TCHAR),
				&cbBytesRead,
				NULL
			);

			if ( !success || cbBytesRead == 0 ) {
				
				if ( GetLastError() == ERROR_BROKEN_PIPE ) {
					// Log that client disconnected
				} else {
					// Log that ReadFile failed
				}

				break;

			}

			getAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);

			// Test Logs //

			std::ofstream fout;
			fout.open("testlogs.txt", std::fstream::out | std::fstream::app);
			fout << "pchRequest: " << pchRequest << std::endl
				 << "pchReply: " << pchReply << std::endl
				 << "cbReplyBytes: " << cbReplyBytes << std::endl;
			fout.close();

			//		//

			Modules::CommandMap::Instance().ExecuteCommand(pchRequest);

			success = WriteFile(
				hPipe,
				pchReply,
				cbReplyBytes,
				&cbWritten,
				NULL
			);

			if ( !success || cbReplyBytes != cbWritten ) {
				// Error log here
				break;
			}

		}

		FlushFileBuffers(hPipe);
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);

		HeapFree(hHeap, 0, pchRequest);
		HeapFree(hHeap, 0, pchReply);

		return 1;

	}

	VOID getAnswerToRequest(LPTSTR pchRequest, LPTSTR pchReply, LPDWORD pchBytes) {

		if (FAILED(StringCchCopy(pchReply, BUFFERSIZE, TEXT("default answer from server"))))
		{
			*pchBytes = 0;
			pchReply[0] = 0;
			// Log StringCchCopy failure, no outgoing message
			return;
		}

		*pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);

	}

	void CommandPipe::run() {

		// Only allow one instance of the pipe manager thread.
		if ( !running_ ) {
			CreateThread(nullptr, 0, startPipeManager, nullptr, 0, nullptr);
			running_ = true;
		}

	}

}