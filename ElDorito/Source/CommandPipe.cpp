#include "CommandPipe.hpp"

namespace Command::Pipe
{

	DWORD WINAPI Command::Pipe::CommandPipe::startNamedPipe(LPVOID lpParam) {



	}

	void Command::Pipe::CommandPipe::run() {

		if ( !running_ ) {
			CreateThread(nullptr, 0, startNamedPipe, nullptr, 0, nullptr);
		}

	}

}