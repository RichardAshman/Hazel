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

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

#define PROFILING 0 // or "PROFILING 0" to turn it off, 1 to turn it on // TODO move to a config file

#if PROFILING
    #define HZ_PROFILE_START(name, filepath) Instrumentor::Get().BeginSession(name, filepath)
    #define HZ_PROFILE_STOP() Instrumentor::Get().EndSession()
    #define HZ_PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(__FUNCSIG__)
    // might need to add another macro with something like "#define COMBINE(x, y) x##y" because some compilers may not like timer##__LINE__
#else
    #define HZ_PROFILE_START(name)
    #define HZ_PROFILE_SCOPE(name)
    #define HZ_PROFILE_STOP()
    #define HZ_PROFILE_FUNCTION()
#endif

struct ProfileResult
{
    std::string Name;
    long long Start, End;
    uint32_t ThreadID;
};

struct InstrumentationSession
{
    std::string Name;
};

class Instrumentor
{
private:
    InstrumentationSession* m_CurrentSession;
    std::ofstream m_OutputStream;
    int m_ProfileCount;
public:
    Instrumentor()
        : m_CurrentSession(nullptr), m_ProfileCount(0)
    {
    }

    void BeginSession(const std::string& name, const std::string& filename = "result.json")
    {
        m_OutputStream.open(filename);
        WriteHeader();
        m_CurrentSession = new InstrumentationSession{ name };
    }

    void EndSession()
    {
        WriteFooter();
        m_OutputStream.close();
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
        m_ProfileCount = 0;
    }

    void WriteProfile(const ProfileResult& result)
    {
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
        m_OutputStream << "\"name\":\"" << name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}";

        m_OutputStream.flush();
    }

    void WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_OutputStream.flush();
    }

    void WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    static Instrumentor& Get()
    {
        static Instrumentor instance;
        return instance;
    }
};

class InstrumentationTimer
{
public:
    InstrumentationTimer(const char* name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

        m_Stopped = true;
    }
private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    bool m_Stopped;
};