#pragma once

#include <YulduzCore/YulduzPCH.hpp>

namespace Yulduz {
    class Timer {
       public:
        using Clock = std::chrono::high_resolution_clock;
        using SystemClock = std::chrono::system_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        using SystemTimePoint = std::chrono::time_point<SystemClock>;
        using Duration = std::chrono::duration<double>;

        Timer() = default;
        ~Timer() = default;

        void start();
        void stop();

        double getElapsedSeconds() const;

        static SystemTimePoint GetCurrentTimePoint();
        static std::string GetCurrentTimeStr();

       protected:
        TimePoint m_Start;
        TimePoint m_End;
    };

    namespace Seconds {
        class Timer : public Yulduz::Timer {
           public:
            double getElapsed() const;
            operator double() const;
        };
    }  // namespace Seconds

    namespace Milliseconds {
        class Timer : public Yulduz::Timer {
           public:
            double getElapsed() const;
            operator double() const;
        };
    }  // namespace Milliseconds

    namespace Microseconds {
        class Timer : public Yulduz::Timer {
           public:
            double getElapsed() const;
            operator double() const;
        };
    }  // namespace Microseconds
    namespace Nanoseconds {
        class Timer : public Yulduz::Timer {
           public:
            double getElapsed() const;
            operator double() const;
        };
    }  // namespace Nanoseconds
}  // namespace Yulduz
