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

    #define HZ_PROFILE_START(name, filepath) Instrumentor::Get().BeginSession(name, filepath)
    #define HZ_PROFILE_STOP() Instrumentor::Get().EndSession()
    #define HZ_PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define HZ_PROFILE_FUNCTION() HZ_PROFILE_SCOPE(HZ_FUNC_SIG)
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
    std::thread::id ThreadID;
};

struct InstrumentationSession
{
    std::string Name;
};

class Instrumentor
{
private:
    std::mutex m_Mutex;
    InstrumentationSession* m_CurrentSession;
    std::ofstream m_OutputStream;
    //int m_ProfileCount;
public:
    Instrumentor()
        : m_CurrentSession(nullptr)//, m_ProfileCount(0)
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
                    HZ_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
                }
                InternalEndSession();
            }
        }
        m_OutputStream.open(filepath);
        //WriteHeader();
        //m_CurrentSession = new InstrumentationSession{ name };
        if (m_OutputStream.is_open()) {
            m_CurrentSession = new InstrumentationSession({ name });
            WriteHeader();
        }
        else {
            if (Hazel::Log::GetCoreLogger()) { // Edge case: BeginSession() might be before Log::Init()
                HZ_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
            }
        }
    }

    void EndSession()
    {
        //WriteFooter();
        //m_OutputStream.close();
        //delete m_CurrentSession;
        //m_CurrentSession = nullptr;
        //m_ProfileCount = 0;
        std::lock_guard lock(m_Mutex);
        InternalEndSession();
    }

    void WriteProfile(const ProfileResult& result)
    {
        /*if (m_ProfileCount++ > 0)
        {
            m_OutputStream << ",";
        }*/
        std::stringstream json;

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        json << ",{";
        json << "\"cat\":\"function\",";
        json << "\"dur\":" << (result.End - result.Start) << ',';
        json << "\"name\":\"" << name << "\",";
        json << "\"ph\":\"X\",";
        json << "\"pid\":0,";
        json << "\"tid\":" << result.ThreadID << ",";
        json << "\"ts\":" << result.Start;
        json << "}";

        std::lock_guard lock(m_Mutex);
        if (m_CurrentSession) {
            m_OutputStream << json.str();
            m_OutputStream.flush();
        }
    }
    static Instrumentor& Get() {
        static Instrumentor instance;
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

    // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
    void InternalEndSession() {
        if (m_CurrentSession) {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
        }
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

        //uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        //Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });
        Instrumentor::Get().WriteProfile({ m_Name, start, end, std::this_thread::get_id() });

        m_Stopped = true;
    }
private:
    const char* m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    bool m_Stopped;
};