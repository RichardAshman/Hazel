// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// PROFILE_START();         // Begin session 
//      PROFILE_FUNCTION(); // Place code like this in scopes you'd like to include in profiling (at the top of each function (and sub) you want to include)
//      // Code             // The code that you are profiling
// PROFILE_STOP();           // End Session
//
// View output json file in google chrome "chrome://tracing"
// Basic setup done by Cherno https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>

#include <string>
#include <thread>

#define HZ_PROFILING 1 // or "PROFILING 0" to turn it off, 1 to turn it on // TODO move to a config file

#if HZ_PROFILING

    // Resolve which function signature macro will be used. Note that this only
    // is resolved when the (pre)compiler starts, so the syntax highlighting
    // could mark the wrong one in your editor!
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
    #define HZ_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
    #define HZ_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__FUNCSIG__)
    #define HZ_FUNC_SIG __FUNCSIG__
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
    #define HZ_FUNC_SIG __FUNCTION__
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
    #define HZ_FUNC_SIG __FUNC__
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
    #define HZ_FUNC_SIG __func__
    #elif defined(__cplusplus) && (__cplusplus >= 201103)
    #define HZ_FUNC_SIG __func__
    #else
    #define HZ_FUNC_SIG "HZ_FUNC_SIG unknown!"
    #endif

    #define HZ_PROFILE_START(name, filepath) Hazel::Profiler::Get().BeginSession(name, filepath)
    #define HZ_PROFILE_STOP() Hazel::Profiler::Get().EndSession()
    #define HZ_PROFILE_SCOPE(name) Hazel::ProfileTimer timer##__LINE__(name)
    #define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(HZ_FUNC_SIG)
    // might need to add another macro with something like "#define COMBINE(x, y) x##y" because some compilers may not like timer##__LINE__
#else
    #define HZ_PROFILE_START(name)
    #define HZ_PROFILE_SCOPE(name)
    #define HZ_PROFILE_STOP()
    #define HZ_PROFILE_FUNCTION()
#endif
namespace Hazel {

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult
    {
        std::string Name;
        FloatingPointMicroseconds Start;
        std::chrono::microseconds ElapsedTime;
        std::thread::id ThreadID;
    };

    struct ProfileSession
    {
        std::string Name;
    };

    class Profiler
    {
    private:
        std::mutex m_Mutex;
        ProfileSession* m_CurrentSession;
        std::ofstream m_OutputStream;

    public:
        Profiler()
            : m_CurrentSession(nullptr)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "result.json")
        {
            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession)
            {
                std::lock_guard lock(m_Mutex);
                if (m_CurrentSession) {
                    // If there is already a current session, then close it before beginning new one.
                    // Subsequent profiling output meant for the original session will end up in the
                    // newly opened session instead.  That's better than having badly formatted
                    // profiling output.
                    if (Hazel::Log::GetCoreLogger()) { // Edge case: BeginSession() might be before Log::Init()
                        HZ_CORE_ERROR("Profiler::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
                    }
                    InternalEndSession();
                }
            }
            m_OutputStream.open(filepath);

            if (m_OutputStream.is_open()) {
                m_CurrentSession = new ProfileSession({ name });
                WriteHeader();
            }
            else {
                if (Hazel::Log::GetCoreLogger()) { // Edge case: BeginSession() might be before Log::Init()
                    HZ_CORE_ERROR("Profiler could not open results file '{0}'.", filepath);
                }
            }
        }

        void EndSession()
        {
            std::lock_guard lock(m_Mutex);
            InternalEndSession();
        }

        void WriteProfile(const ProfileResult& result)
        {
            std::stringstream json;

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            json << std::setprecision(3) << std::fixed;
            json << ",{";
            json << "\"cat\":\"function\",";
            json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
            json << "\"name\":\"" << name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start.count();
            json << "}";

            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession) {
                m_OutputStream << json.str();
                m_OutputStream.flush();
            }
        }
        static Profiler& Get() {
            static Profiler instance;
            return instance;
        }
    private:

        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        // Note: you must already own lock on m_Mutex before calling InternalEndSession()
        void InternalEndSession() {
            if (m_CurrentSession) {
                WriteFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
            }
        }
    };


    class ProfileTimer
    {
    public:
        ProfileTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();
        }

        ~ProfileTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::steady_clock::now();

            auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            Profiler::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

            m_Stopped = true;
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}