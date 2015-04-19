#include "Core.h"

#include "../ElDorito.h"
#include "../Patch.h"

namespace
{
	void GameTickHook(int frames, float *deltaTimeInfo);
}

namespace Patches
{
	namespace Core
	{
		void ApplyAll()
		{
			// Enable tag edits
			Patch(0x101A5B, { 0xEB }).Apply();
			Patch::NopFill(Pointer::Base(0x102874), 2);

			// No --account args patch
			Patch(0x43731A, { 0xEB, 0x0E }).Apply();
			Patch(0x4373AD, { 0xEB, 0x03 }).Apply();

			// Remove preferences.dat hash check
			Patch::NopFill(Pointer::Base(0x10C99A), 0x6);

			// Patch to allow spawning AI through effects
			Patch::NopFill(Pointer::Base(0x1033321), 2);

			// Prevent game variant weapons from being overridden
			Pointer::Base(0x1A315F).Write<uint8_t>(0xEB);
			Pointer::Base(0x1A31A4).Write<uint8_t>(0xEB);

			// Hook game ticks
			Hook(0x105E64, true, GameTickHook).Apply();
		}
	}
}

namespace
{
	void GameTickHook(int frames, float *deltaTimeInfo)
	{
		// Tick ElDorito
		float deltaTime = *deltaTimeInfo;
		ElDorito::Instance().Tick(std::chrono::duration<double>(deltaTime));

		// Tick the game
		typedef void (*GameTickFunc)(int frames, float *deltaTimeInfo);
		GameTickFunc GameTick = reinterpret_cast<GameTickFunc>(0x5336F0);
		GameTick(frames, deltaTimeInfo);
	}
}