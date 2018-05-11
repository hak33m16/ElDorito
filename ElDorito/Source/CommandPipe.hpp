#include <Windows.h>
#include <thread>
#include <string>

#include "Modules/ModuleGame.hpp"
#include "Utils/Singleton.hpp"

namespace Command::Pipe
{

	class CommandPipe : public Utils::Singleton<CommandPipe>
	{
	public:
		inline CommandPipe() : running_(false), pipeName_("\\\\.\\pipe\\Pipe") { }

		void run();

	private:
		static DWORD WINAPI startNamedPipe(LPVOID);
		HANDLE hPipe_;

		std::string pipeName_;
		bool running_;

	};

}