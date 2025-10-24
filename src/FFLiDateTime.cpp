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


/*
void FFLiDateTime2DateTimeHidden
               (FFLiDateTimeHidden *pDateTimeHidden, const FFLiDateTime& dateTime)

{
    uint uVar1;
    uint uVar2;
    int iVar3;
    uint uVar4;
    uint uVar5;

    iVar3 = FFLiClamp__tm__2_i__FZ1ZN21_Z1Z(2000,0x84f,dateTime.year);
    uVar4 = *pDateTimeHidden;
    uVar1 = (*(ushort *)((int)dateTime.month + 2) & 0xf | (iVar3 + -2000) * 0x10 & 0xffff0U) << 0x15
    ;
    *pDateTimeHidden = uVar1 | uVar4 & 0x1fffff;
    uVar5 = dateTime.second;
    uVar2 = ((*(ushort *)((int)dateTime.day + 2) & 0x1f) << 5 |
            *(ushort *)((int)dateTime.hour + 2) & 0x1f) << 0xb;
    *pDateTimeHidden = uVar2 | uVar1 | uVar4 & 0x7ff;
    *pDateTimeHidden =
        (*(ushort *)((int)dateTime.minute + 2) & 0x3f) << 5 |
        ((int)uVar5 >> 1) + (uint)((int)uVar5 < 0 && (uVar5 & 1) != 0) & 0x1f | uVar2 | uVar1;

    // AFL:
    uVar7 = *(uint *)pDateTimeHidden;
    *(uint *)pDateTimeHidden = uVar5 | uVar7 & 0xffffff80;
                    // month
    uVar1 = (*(uint *)(&dateTime + 4) & 0xf) << 7;
    *(uint *)pDateTimeHidden = uVar5 | uVar7 & 0xfffff800 | uVar1;
                    // day
    uVar2 = (*(uint *)(&dateTime + 8) & 0x1f) << 0xb;
    *(uint *)pDateTimeHidden = uVar5 | uVar7 & 0xffff0000 | uVar1 | uVar2;
                    // hour
    uVar3 = (*(uint *)(&dateTime + 0xc) & 0x1f) << 0x10;
    *(uint *)pDateTimeHidden = uVar5 | uVar7 & 0xffe00000 | uVar1 | uVar2 | uVar3;
                    // minute
    uVar4 = (*(uint *)(&dateTime + 0x10) & 0x3f) << 0x15;
    *(uint *)pDateTimeHidden = uVar5 | uVar7 & 0xf8000000 | uVar1 | uVar2 | uVar3 | uVar4;
                    // second
    *(uint *)pDateTimeHidden =
        uVar5 | uVar1 | uVar2 | uVar3 | uVar4 |
        (*(int *)(&dateTime + 0x14) - (*(int *)(&dateTime + 0x14) >> 0x1f)) * 0x4000000 & 0xf8000000U;
}

void AFLiDateTimeHidden2DateTime(AFLiDateTime *pDateTime,const AFLiDateTimeHidden& dateTimeHidden)

{
    // FFL:
    pDateTime->year = (dateTimeHidden >> 0x19) + 2000;
    pDateTime->month = dateTimeHidden >> 0x15 & 0xf;
    pDateTime->day = dateTimeHidden >> 0x10 & 0x1f;
    pDateTime->hour = dateTimeHidden >> 0xb & 0x1f;
    pDateTime->minute = dateTimeHidden >> 5 & 0x3f;
    pDateTime->second = (dateTimeHidden & 0x1f) << 1;

    // AFL:
    pDateTime->year = (*(uint *)&dateTimeHidden & 0x7f) + 2000;
    pDateTime->month = (uint)(*(int *)&dateTimeHidden << 0x15) >> 0x1c;
    pDateTime->day = (uint)(*(int *)&dateTimeHidden << 0x10) >> 0x1b;
    pDateTime->hour = *(ushort *)(&dateTimeHidden + 2) & 0x1f;
    pDateTime->minute = (uint)(*(int *)&dateTimeHidden << 5) >> 0x1a;
    pDateTime->second = *(uint *)&dateTimeHidden >> 0x1a & 0x3e;
}
*/
