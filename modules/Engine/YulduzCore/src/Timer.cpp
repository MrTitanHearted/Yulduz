#include <YulduzCore/Timer.hpp>

namespace Yulduz {
    void Timer::start() {
        m_Start = Clock::now();
    }

    void Timer::stop() {
        m_End = Clock::now();
    }

    double Timer::getElapsedSeconds() const {
        if (!m_Start || !m_End) return 0.0f;

        return Timer::Duration(m_End.value() - m_Start.value()).count();
    }

    Timer::SystemTimePoint Timer::GetCurrentTimePoint() {
        return SystemClock::now();
    }

    std::string Timer::GetCurrentTimeStr() {
        SystemTimePoint now = SystemClock::now();
        std::time_t time = SystemClock::to_time_t(now);
        std::tm *tm = std::localtime(&time);

        tm->tm_year += 1900;
        tm->tm_mon += 1;

        return std::vformat("{0:04d}-{1:02d}-{2:02d} {3:02d}:{4:02d}:{5:02d}",
                            std::make_format_args(tm->tm_year,
                                                  tm->tm_mon,
                                                  tm->tm_mday,
                                                  tm->tm_hour,
                                                  tm->tm_min,
                                                  tm->tm_sec));
    }

    double Seconds::Timer::getElapsed() const {
        if (!m_Start || !m_End) return 0.0f;

        return Timer::Duration(m_End.value() - m_Start.value()).count();
    }

    Seconds::Timer::operator double() const {
        if (!m_Start || !m_End) return 0.0f;

        return Timer::Duration(m_End.value() - m_Start.value()).count();
    }

    double Milliseconds::Timer::getElapsed() const {
        if (!m_Start || !m_End) return 0.0f;

        return std::chrono::duration<double, std::milli>(m_End.value() - m_Start.value()).count();
    }

    Milliseconds::Timer::operator double() const {
        if (!m_Start || !m_End) return 0.0f;

        return std::chrono::duration<double, std::milli>(m_End.value() - m_Start.value()).count();
    }

    double Microseconds::Timer::getElapsed() const {
        if (!m_Start || !m_End) return 0.0f;

        return std::chrono::duration<double, std::micro>(m_End.value() - m_Start.value()).count();
    }

    Microseconds::Timer::operator double() const {
        if (!m_Start || !m_End) return 0.0f;

        return std::chrono::duration<double, std::micro>(m_End.value() - m_Start.value()).count();
    }

    double Nanoseconds::Timer::getElapsed() const {
        if (!m_Start || !m_End) return 0.0f;

        return std::chrono::duration<double, std::nano>(m_End.value() - m_Start.value()).count();
    }

    Nanoseconds::Timer::operator double() const {
        if (!m_Start || !m_End) return 0.0f;

        return std::chrono::duration<double, std::nano>(m_End.value() - m_Start.value()).count();
    }
}  // namespace Yulduz
