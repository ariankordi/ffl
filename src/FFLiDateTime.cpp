#include <nn/ffl/FFLiDateTime.h>

#if RIO_IS_CAFE
#include <cafe/os.h>
#else
#if RIO_IS_WIN
#include <misc/win/rio_Windows.h>
#endif // RIO_IS_WIN
#include <ctime>
#endif

s64 FFLiGetTick()
{
#if RIO_IS_CAFE
    return OSGetTime();
#elif RIO_IS_WIN
    LARGE_INTEGER ticks;
    [[maybe_unused]] WINBOOL success = QueryPerformanceFrequency(&ticks);
    RIO_ASSERT(success);
    return ticks.QuadPart;
#else
    return 0;
#endif
}

void FFLiGetDateTimeNow(FFLiDateTime* pDateTime)
{
#if RIO_IS_CAFE
    OSCalendarTime calendarTime;
    OSTicksToCalendarTime(FFLiGetTick(), &calendarTime);

    pDateTime->year = calendarTime.year;
    pDateTime->month = calendarTime.mon + 1;
    pDateTime->day = calendarTime.mday;
    pDateTime->hour = calendarTime.hour;
    pDateTime->minute = calendarTime.min;
    pDateTime->second = calendarTime.sec;
#else
    std::time_t t = std::time(nullptr);
    const std::tm& tm = *std::localtime(&t);

    pDateTime->year = tm.tm_year + 1900;
    pDateTime->month = tm.tm_mon + 1;
    pDateTime->day = tm.tm_mday;
    pDateTime->hour = tm.tm_hour;
    pDateTime->minute = tm.tm_min;
    pDateTime->second = tm.tm_sec;
#endif // RIO_IS_CAFE
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
