//-----------------------------------------------------------------------------
// sTest - unit testing framework for C++
//-----------------------------------------------------------------------------

// example of use:
/*
#include "test.h"

void test_sub()
{
    TEST_GROUP_FUNCTION;
    TEST(1 - 1 == 1);
    TEST(1 - 2 == -1);
}

int main()
{
    try
    {
        TEST_GROUP("test_add");
        TEST(1 + 1 == 2);
        TEST(1 + 2 == 3);

        test_sub();

        TEST_GROUP("test_mul");
        TEST(1 * 1 == 1);

        TEST_MERGE(true);
        TEST(1 * 2 == 2);
        TEST(2 * 1 == 1);

        TEST_SUMMARY;
    }
    catch (...)
    {
        TEST_EXCEPTION;
    }
    return 0;
}
*/

//-----------------------------------------------------------------------------

#pragma once

#if !defined(__cplusplus)
#error C++ is required
#endif

#include <cstring>  // strrchr
#include <cstdlib>  // used for exit(EXIT_FAILURE)

// get source file name without path
#if defined(_WIN32) && defined(_MSC_VER)
#define __FILENAME__        (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__        (std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

// 
#define TEST(X)             _test::log<int>(_test::check, #X, __FILENAME__, __LINE__, (X))
#define TEST_R(X)            if(!_test::log<int>(_test::check_skip, #X, __FILENAME__, __LINE__, (X))) return;
#define TEST_IF(X)          _test::log<int>(_test::check_skip, #X, __FILENAME__, __LINE__, (X))

#define TEST_MERGE(X)       _test::log<int>(_test::merge, nullptr, nullptr, 0, (X))

#define TEST_SUMMARY        _test::log<int>(_test::summary)
#define TEST_EXCEPTION      _test::log<int>(_test::exception)

// aditional macros
#define TEST_GROUP(X)       _test::log<int>(_test::begin_group, (X))
#define TEST_GROUP_FUNCTION _test::log<int>(_test::begin_group, __func__)

#define TEST_PRINT(X)       _test::log<int>(_test::print, (X))

#define TEST_FAILED         _test::log<int>(_test::total_failed)

// options
#define TEST_EXIT(X)        _test::log<int>(_test::do_exit, nullptr, nullptr, 0, (X))
#define TEST_WAIT(X)        _test::log<int>(_test::do_wait, nullptr, nullptr, 0, (X))


namespace _test
{
    struct Status
    {
        long long testCount, failedCount;
        bool hasSkipped;

        Status()
        {
            Clear();
        }

        void Clear()
        {
            testCount = 0;
            failedCount = 0;
            hasSkipped = false;
        }
    };


    struct MergedInfo
    {
        bool counted;
        bool failed;
        bool printed;

        MergedInfo()
        {
            Clear();
        }

        void Clear()
        {
            counted = false;
            failed = false;
            printed = false;
        }
    };


    struct Options
    {
        bool exitAtEnd;
        bool waitAtExit;

        Options()
        {
            exitAtEnd = true;
            waitAtExit = true;
        }
    };


    enum LogType
    {
        check,
        check_skip,
        summary,
        exception,
        begin_group,
        print,
        merge,
        total_failed,
        do_exit,
        do_wait,
    };


    template <typename T>
    bool log(LogType type, const char *what = nullptr, const char *file = nullptr, int line = 0, bool passed = false)
    {
        static Status totalStatus, groupStatus;

        static bool isMerged;
        static MergedInfo merged;

        static const char *lastFile;
        static int lastLine;

        static Options options;

        static bool hasTests = false;
        static bool info = false;

        if (!info)
        {
            print_info<T>();
            info = true;
        }

        switch (type)
        {
            case LogType::check:
            case LogType::check_skip:
                hasTests = true;
                if (!isMerged || !merged.counted)
                {
                    totalStatus.testCount++;
                    groupStatus.testCount++;
                    merged.counted = isMerged;
                }
                lastFile = file;
                lastLine = line;
                if (!passed)
                {
                    if (!isMerged || !merged.failed)
                    {
                        totalStatus.failedCount++;
                        groupStatus.failedCount++;
                        merged.failed = isMerged;
                    }
                    if (type == LogType::check_skip)
                    {
                        totalStatus.hasSkipped = true;
                        groupStatus.hasSkipped = true;
                    }
                }

                merged.printed = print_test_check<T>(what, file, line, passed, type == LogType::check_skip, merged.printed) && isMerged;
                break;

            case LogType::summary:
            {
                if (hasTests)
                    print_group_status<T>(groupStatus.failedCount, groupStatus.testCount, groupStatus.hasSkipped);
                print_summary<T>(totalStatus.failedCount, totalStatus.testCount, totalStatus.hasSkipped);

                bool testIsFailed = totalStatus.failedCount;

                totalStatus.Clear();
                groupStatus.Clear();
                isMerged = false;
                merged.Clear();

                if (options.exitAtEnd)
                {
                    if (options.waitAtExit)
                        wait<T>();
                    if (testIsFailed)
                        exit(EXIT_FAILURE);
                    else
                        exit(EXIT_SUCCESS);
                }
                break;
            }

            case LogType::begin_group:
                if (what)
                {
                    if (hasTests)
                        print_group_status<T>(groupStatus.failedCount, groupStatus.testCount, groupStatus.hasSkipped);
                    print_test_group<T>(what);

                    groupStatus.Clear();
                    isMerged = false;
                    merged.Clear();
                }
                break;

            case LogType::exception:
                print_exception<T>(lastFile, lastLine);

                totalStatus.Clear();
                groupStatus.Clear();
                isMerged = false;
                merged.Clear();

                if (options.exitAtEnd)
                {
                    if (options.waitAtExit)
                        wait<T>();
                    exit(EXIT_FAILURE);
                }
                break;

            case LogType::merge:
                isMerged = passed;
                merged.Clear();
                break;

            case LogType::print:
                if (what)
                    print_print<T>(what);
                break;

            case LogType::total_failed:
                return totalStatus.failedCount != 0;

            case LogType::do_exit:
                options.exitAtEnd = passed;
                break;

            case LogType::do_wait:
                options.waitAtExit = passed;
                break;

            default:
                break;
        }

        return passed;
    }
}


#include <iostream> // used for console output

namespace _test
{
    template <typename T>
    void print_info()
    {
        std::cout << "sTest v 1.0 "; 
        std::cout << "<console:text>\n";
        std::cout << std::endl;
    }


    template <typename T>
    void print_test_group(const char *name)
    {
        std::cout << name;
        std::cout << std::endl;
    }

    // terurn ture if printed
    template <typename T>
    bool print_test_check(const char *what, const char *file, int line, bool passed, bool skip, bool merged)
    {
        if (!passed)
        {
            if (merged)
                std::cout << "      failed!";
            else
                std::cout << " Test failed!";
            std::cout << "   " << file;
            std::cout << ":" << line;
            std::cout << "   " << what;
            if (skip)
                std::cout << "\n -skipping next tests";
            std::cout << std::endl;
            return true;
        }
        return false;
    }


    template <typename T>
    void print_group_status(long long testFailedCount, long long testCunt, bool testWasSkipped)
    {
        if (testFailedCount)
            std::cout << " -failed: " << testFailedCount << " of " << testCunt;
        else
            std::cout << " -test count: " << testCunt;
        if (testWasSkipped)
            std::cout << "*";
        std::cout << std::endl;
    }


    template <typename T>
    void print_summary(long long totalFailedCount, long long totalCount, bool totalWasSkipped)
    {
        std::cout << "\n==============================\n";
        if (totalFailedCount == 0)
            std::cout << "All tests passed!\n";
        else
            std::cout << "Warning " << totalFailedCount << " tests failed!\n";
        std::cout << "Test count: " << totalCount;
        if (totalWasSkipped)
            std::cout << "*\n*Some tests may be skipped.";
        std::cout << std::endl;
    }


    template <typename T>
    void print_exception(const char *lastFile, int lastLine)
    {
        std::cout << "\n==============================\n";
        if (lastFile)
        {
            std::cout << "Exception afer test in: ";
            std::cout << lastFile;
            std::cout << ":" << lastLine;
        }
        else
            std::cout << "Exception beafore anny test!";
        std::cout << std::endl;
    }


    template <typename T>
    void print_print(const char *txt)
    {
        std::cout << txt;
        std::cout << std::endl;
    }


    template <typename T>
    void wait()
    {
        std::cin.get();
    }
}

