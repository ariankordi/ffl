#include <nn/ffl/FFLiDateTime.h>

#if RIO_IS_CAFE
#include <coreinit/time.h>
#elif RIO_IS_WIN && defined(_WIN32)
#include <misc/win/rio_Windows.h>
#elif RIO_IS_WIN
#include <time.h>
#include <cstdint>
#endif // RIO_IS_WIN && defined(_WIN32)
#include <ctime>

s64 FFLiGetTick()
{
#if RIO_IS_CAFE
    return OSGetTime();
#elif RIO_IS_WIN && defined(_WIN32)
    LARGE_INTEGER ticks;
    [[maybe_unused]] bool success = QueryPerformanceFrequency(&ticks);
    RIO_ASSERT(success);
    return ticks.QuadPart;
#elif RIO_IS_WIN
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<s64>(ts.tv_sec) * 1000000000 + ts.tv_nsec;
#else
    return 0;
#endif
}

void FFLiGetDateTimeNow(FFLiDateTime* pDateTime)
{
#if RIO_IS_CAFE
    OSCalendarTime calendarTime;
    OSTicksToCalendarTime(FFLiGetTick(), &calendarTime);

    pDateTime->year = calendarTime.tm_year;
#else
    std::time_t t = std::time(nullptr);
    const std::tm& calendarTime = *std::localtime(&t);

    pDateTime->year = calendarTime.tm_year + 1900;
#endif // RIO_IS_CAFE
    pDateTime->month = calendarTime.tm_mon + 1;
    pDateTime->day = calendarTime.tm_mday;
    pDateTime->hour = calendarTime.tm_hour;
    pDateTime->minute = calendarTime.tm_min;
    pDateTime->second = calendarTime.tm_sec;
}

void FFLiGetNowDateTime(s32* pYear, s32* pMonth, s32* pDay, s32* pHour, s32* pMinute, s32* pSecond)
{
    FFLiDateTime dateTime;
    FFLiGetDateTimeNow(&dateTime);

    *pYear = dateTime.year;
    *pMonth = dateTime.month;
    *pDay = dateTime.day;
    *pHour = dateTime.hour;
    *pMinute = dateTime.minute;
    *pSecond = dateTime.second;
}

bool FFLiIsLeapYear(s32 year)
{
    if (year % 400 == 0)
        return true;

    if (year % 100 != 0 && year % 4 == 0)
        return true;

    return false;
}

static const s32 MONTH_OF_DAY[12 + 1] = {
    0,
    31, // January
    29, // February
    31, // March
    30, // April
    31, // May
    30, // June
    31, // July
    31, // August
    30, // September
    31, // October
    30, // November
    31  // December
};

s32 FFLiGetMonthOfDay(s32 month)
{
    if (1 <= month && month <= 12)
        return MONTH_OF_DAY[month];

    return 0;
}

s32 FFLiDateToDays(s32 year, s32 month, s32 day)
{
    year -= 2000;
    day--;

    if (month <= 2)
    {
        year--;
        month += 9;
    }
    else
    {
        month -= 3;
    }

    s32 leap_year_add_day = 1;
    if (year < 0)
        leap_year_add_day = s32(FFLiIsLeapYear(year));

    s32 ret  = year / 100 * 146097 / 4;
    ret     += year % 100 * 1461  / 4;
    ret     += (month * 153 + 2) / 5;
    ret     += day;
    ret     += leap_year_add_day;
    ret     += 59;

    return ret;
}

s32 FFLiGetSecondsFrom_2000_01_01()
{
    s32 year, month, day, hour, minute, second;
    FFLiGetNowDateTime(&year, &month, &day, &hour, &minute, &second);

    s32 days = FFLiDateToDays(year, month, day);
    return ((days * 24 + hour) * 60 + minute) * 60 + second;
}
