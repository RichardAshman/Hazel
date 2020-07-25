#pragma once

#include "Hazel/Core/Base.h"

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv)
{
	Hazel::Log::Init();

	HZ_PROFILE_START("Startup", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_STOP();

	HZ_PROFILE_START("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_STOP();

	HZ_PROFILE_START("Shutdown", "HazelProfile-Shutdown.json");
	delete app;
	HZ_PROFILE_STOP();
}
#endif