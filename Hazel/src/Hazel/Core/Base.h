#pragma once

#include <memory>


// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
	#define HZ_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define HZ_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define HZ_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
 /* We also have to check __ANDROID__ before __linux__
  * since android is based on the linux kernel
  * it has __linux__ defined */
#elif defined(__ANDROID__)
#define HZ_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define HZ_PLATFORM_LINUX
#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
#error "Unknown platform!"
#endif // End of platform detection


//// DLL support
//#ifdef HZ_PLATFORM_WINDOWS
//	#if HZ_DYNAMIC_LINK
//		#ifdef HZ_BUILD_DLL
//			#define HAZEL_API __declspec(dllexport)
//		#else
//			#define HAZEL_API __declspec(dllimport)
//		#endif
//	#else
//		#define HAZEL_API
//	#endif
//#else
//	#error Hazel only supports Windows!
//#endif // End of DLL support


#ifdef HZ_DEBUG	
	#ifdef HZ_PLATFORM_WINDOWS
		#define HZ_DEBUGBREAK() __debugbreak()
	#elif defined(HZ_PLATFORM_LINUX)
		#include <signal.h>
		#define HZ_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif

	#define HZ_ENABLE_ASSERTS
	#define HZ_PROFILING 0 // or "PROFILING 0" to turn it off, 1 to turn it on // TODO move to a config file
	#define HZ_LOG_EVENTS_TO_CONSOLE 0
#else
#define HZ_DEBUGBREAK()
#endif

#define HZ_EXPAND_MACRO(x) x
#define HZ_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

//#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this, std::placeholders::_1)
#define HZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Hazel {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args) ...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args) ...);
	}
}

#include "Hazel/Core/Log.h"
#include "Hazel/Core/Assert.h"

	// When using shared (or unique) pointers the code will change
	// From 
	// std::shared_ptr<Shader>
	// to
	// Ref<Shader> or Hazel::Ref<Shader>
	// So that if we need to change it sometime we can just change it here and everything will be changed.