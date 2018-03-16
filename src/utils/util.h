// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/**
 * Server/client environment: argument handling, config file parsing,
 * logging, thread wrappers, startup time
 */
#ifndef BITCOIN_UTIL_H
#define BITCOIN_UTIL_H

#if defined(HAVE_CONFIG_H)

#include "config/sbtc-config.h"

#endif
#include <log4cpp/Category.hh>
#include "base/base.hpp"
#include "compat/compat.h"
#include "fs.h"
#include "framework/sync.h"
#include "tinyformat.h"
#include "utiltime.h"

#include <atomic>
#include <exception>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include <boost/signals2/signal.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>
#include "config/argmanager.h"

#define USE_LOG4CPP

namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;
using std::vector;
using std::string;

extern const char *const BITCOIN_CONF_FILENAME;
extern const char *const BITCOIN_PID_FILENAME;

// Application startup time (used for uptime calculation)
int64_t GetStartupTime();

//static const bool DEFAULT_LOGTIMEMICROS = false;
//static const bool DEFAULT_LOGIPS = false;
//static const bool DEFAULT_LOGTIMESTAMPS = true;
//static const bool DEFAULT_LOGFILEINFO = true;

/** Signals for translation. */
class CTranslationInterface
{
public:
    /** Translate a message to the native language of the user. */
    boost::signals2::signal<std::string(const char *psz)> Translate;
};

extern CTranslationInterface translationInterface;

/**
 * Translation function: Call Translate signal on UI interface, which returns a boost::optional result.
 * If no translation slot is registered, nothing is returned, and simply return the input.
 */
inline std::string _(const char *psz)
{
    boost::optional<std::string> rv = translationInterface.Translate(psz);
    return rv ? (*rv) : psz;
}


//extern bool fPrintToConsole;
//extern bool fPrintToDebugLog;
//extern bool fLogTimestamps;
//extern bool fLogTimeMicros;
//extern bool fLogIPs;
//extern std::atomic<bool> fReopenDebugLog;
//
//extern std::atomic<uint32_t> logCategories;

namespace BCLog
{
    enum LogFlags : uint32_t
    {
        NONE = 0,
        NET = (1 << 0),
        TOR = (1 << 1),
        MEMPOOL = (1 << 2),
        HTTP = (1 << 3),
        BENCH = (1 << 4),
        ZMQ = (1 << 5),
        DB = (1 << 6),
        RPC = (1 << 7),
        ESTIMATEFEE = (1 << 8),
        ADDRMAN = (1 << 9),
        SELECTCOINS = (1 << 10),
        REINDEX = (1 << 11),
        CMPCTBLOCK = (1 << 12),
        RAND = (1 << 13),
        PRUNE = (1 << 14),
        PROXY = (1 << 15),
        MEMPOOLREJ = (1 << 16),
        LIBEVENT = (1 << 17),
        COINDB = (1 << 18),
        QT = (1 << 19),
        LEVELDB = (1 << 20),
        CKECKPOINT = (1 << 21),
        ALL = ~(uint32_t)0,
    };
}

//struct CLogCategoryActive
//{
//    std::string category;
//    bool active;
//};
///** Return true if log accepts specified category */
static inline bool LogAcceptCategory(uint32_t category)
{
    return true;
    //return (logCategories.load(std::memory_order_relaxed) & category) != 0;
}

///** Returns a string with the log categories. */
inline std::string ListLogCategories() { return std::string(); }

///** Returns a vector of the active log categories. */
//std::vector<CLogCategoryActive> ListActiveLogCategories();
//
///** Return true if str parses as a log category and set the flags in f */
//bool GetLogCategory(uint32_t *f, const std::string *str);
//
///** Send a string to the log output */
//int LogPrintStr(const std::string &str);
//
///** Get format string from VA_ARGS for error reporting */
//template<typename... Args>
//std::string FormatStringFromLogArgs(const char *fmt, const Args &... args)
//{
//    return fmt;
//}

//static inline void MarkUsed()
//{
//}
//
//template<typename T, typename... Args>
//static inline void MarkUsed(const T &t, const Args &... args)
//{
//    (void)t;
//    MarkUsed(args...);
//}




#ifdef USE_LOG4CPP

log4cpp::Category &LogFlag2Log4CppObj(int logFlag);
inline std::string int2string(int line)
{
    std::ostringstream oss;
    oss << "("<< line <<")";
    return oss.str();
}





#define  LogPrintf_(logger, fmt, a...) logger.notice(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#define  LogPrintf(fmt, a...) LogPrintf_(LogFlag2Log4CppObj(0), fmt, ##a)
#define  LogPrint(category, a...) LogPrintf_(LogFlag2Log4CppObj(category), ##a)
#define _TXT__(x) #x
#define EMTOSTR(EM) _TXT__(EM)
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
static log4cpp::Category *mlog = & log4cpp::Category::getInstance("CID_APP");

#define DECLEAR_CATEOGRY(CATE)  mlog = & log4cpp::Category::getInstance(#CATE);
#define  mlog_notice(fmt, a...) mlog->notice(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#define  mlog_error(fmt, a...) mlog->error(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#define  mlog_info(fmt, a...) mlog->info(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#define  mlog_warn(fmt, a...) mlog->warn(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#define  mlog_fatal(fmt, a...) mlog->fatal(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#define  mlog_debug(fmt, a...) mlog->debug(tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
//#define  error(fmt, a...)   asdfasdf("%s",tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())

template<typename... Args>
bool error(const char *fmt, const Args &... args)
{
    mlog->error(tinyformat::format(fmt, args...).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str());
    return false;
}
#else

#define  LogPrintf_(logger, fmt, a...)
#define  LogPrintf(fmt, a...)
#define  LogPrint(category, a...)
#define  mlog_notice(fmt, a...)
#define  mlog_error(fmt, a...)
#define  mlog_info(fmt, a...)
#define  mlog_warn(fmt, a...)
#define  mlog_fatal(fmt, a...)
#define  mlog_debug(fmt, a...)
#define  error(fmt, a...)   error2("%s",tinyformat::format(fmt, ##a).append(" ").append(__FILENAME__).append(int2string(__LINE__)).c_str())
#endif



//static  char const * category = "blockchain";






void SetupEnvironment();

bool SetupNetworking();

void PrintExceptionContinue(const std::exception *pex, const char *pszThread);

void FileCommit(FILE *file);

bool TruncateFile(FILE *file, unsigned int length);

int RaiseFileDescriptorLimit(int nMinFD);

void AllocateFileRange(FILE *file, unsigned int offset, unsigned int length);

bool RenameOver(fs::path src, fs::path dest);

bool TryCreateDirectories(const fs::path &p);

fs::path GetDefaultDataDir();

const fs::path &GetDataDir(bool fNetSpecific = true);

void ClearDatadirCache();

fs::path GetConfigFile(const std::string &confPath);

#ifndef WIN32

fs::path GetPidFile();

void CreatePidFile(const fs::path &path, pid_t pid);

#endif
#ifdef WIN32
fs::path GetSpecialFolderPath(int nFolder, bool fCreate = true);
#endif

void runCommand(const std::string &strCommand);

inline bool IsSwitchChar(char c)
{
#ifdef WIN32
    return c == '-' || c == '/';
#else
    return c == '-';
#endif
}


/**
 * Format a string to be used as group of options in help messages
 *
 * @param message Group name (e.g. "RPC server options:")
 * @return the formatted string
 */
std::string HelpMessageGroup(const std::string &message);

/**
 * Format a string to be used as option description in help messages
 *
 * @param option Option message (e.g. "-rpcuser=<user>")
 * @param message Option description (e.g. "Username for JSON-RPC connections")
 * @return the formatted string
 */
std::string HelpMessageOpt(const std::string &option, const std::string &message);

/**
 * Return the number of physical cores available on the current system.
 * @note This does not count virtual cores, such as those provided by HyperThreading
 * when boost is newer than 1.56.
 */
int GetNumCores();

void RenameThread(const char *name);

/**
 * .. and a wrapper that just calls func once
 */
template<typename Callable>
void TraceThread(const char *name, Callable func)
{
    std::string s = strprintf("sbtc-%s", name);
    RenameThread(s.c_str());
    try
    {
        mlog_info("%s thread start\n", name);
        func();
        mlog_info("%s thread exit\n", name);
    }
    catch (const boost::thread_interrupted &)
    {
        mlog_info("%s thread interrupt\n", name);
        throw;
    }
    catch (const std::exception &e)
    {
        PrintExceptionContinue(&e, name);
        throw;
    }
    catch (...)
    {
        PrintExceptionContinue(nullptr, name);
        throw;
    }
}


void
GenerateOptFormat(const int &argc, const char **argv, vector<string> &argv_arr_tmp, vector<const char *> &argv_arr);

std::string CopyrightHolders(const std::string &strPrefix);

#endif // BITCOIN_UTIL_H
