#pragma once

#include <Windows.h>
#include <strsafe.h>
#include <thread>
#include <string>
#include <fstream>

#include "Modules/ModuleGame.hpp"
#include "Utils/Singleton.hpp"

namespace Command::Pipe
{

	const int BUFFERSIZE = 4096;
	// Pipe Naming Convention: \\[server](. = local)\pipe\[pipename]
	const LPSTR pipeName = TEXT("\\\\.\\pipe\\eldewritopipe");

	DWORD WINAPI startPipeManager(LPVOID);
	DWORD WINAPI handlePipeThread(LPVOID);
	VOID getAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

	class CommandPipe : public Utils::Singleton<CommandPipe>
	{
	public:
		inline CommandPipe()
			: running_(false) { }

		void run();

	private:
		bool running_;

	};

}