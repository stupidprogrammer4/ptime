#include "ptime.h"


char *nameOfDays[] = {
    "یکشنبه", "دوشنبه", "سشنبه", "چهارشنبه", "پنجشنبه", "جمعه", "شنبه"
};

static int divider(int num, int den) {
    if (num > 0) 
		return num % den;
	return num - ((((num + 1) / den) - 1) * den);
}

static int getJdn(int year, int mon, int mday) {
    int base = year - 473;
    if (year >= 0) base--;

    int epy = 474 + (base % 2820);

    int md;

    if (mon <= 7) 
        md = (mon-1)*31;
    else
        md = (mon-1)*30+6; 

    return mday + md + (epy*682-110)/2816 + (epy-1)*365 + base/2820*1029983 + 1948320;
}

static int isLeapYear(int year) {
    return divider(25*year+11, 33) < 8;
}


static int calcultateYday(ptime *pt) {
    int months[] = {
        0, 31, 62, 93, 124, 155, 186, 216, 246, 276, 306, 336
    };
    return months[pt->pt_mon-1]+pt->pt_mday;
}

gtime *convertToGregourian(ptime *pt) {
    int year, mon, mday;
    int jdn = getJdn(pt->pt_year, pt->pt_mon, pt->pt_mday);
    if (jdn > 2299160) {
        long long l = jdn + 68569;
        int n = 4 * l / 146097;
        l = l - (146097 * n + 3) / 4;
        int i = 4000 * (l + 1) / 1461001;
        l = l - 1461 * i / 4 + 31;
        int j = 80 * l / 2447;
        mday = l - 2447 * j / 80;
        l = j / 11;
        mon = j + 2 - 12 * l;
        year = 100 * (n - 49) + i + l;
    } 
    else {
        int j = jdn + 1402;
        int k = (j - 1) / 1461;
        long long l = j - 1461 * k;
        int n = (l - 1) / 365 - l / 1461;
        int i = l - 365 * n + 30;
        j = 80 * i / 2447;
        mday = i - 2447 * j / 80;
        i = j / 11;
        mon = j + 2 - 12 * i;
        year = 4 * k + n + i - 4716;
    }

    gtime *gt = malloc(sizeof(struct tm));
    
    gt->tm_hour = pt->pt_hour;
    gt->tm_min = pt->pt_min;
    gt->tm_sec = pt->pt_sec;
    gt->tm_year = year-1900;
    gt->tm_mon = mon-1;
    gt->tm_mday = mday;
    gt->tm_isdst = pt->pt_isdst;

    mktime(gt);

    return gt;
}

ptime *convertToJalali(gtime *gt) {
    ptime *pt = malloc(sizeof(struct PTIME_));

    pt->pt_wday = gt->tm_wday;
    pt->pt_hour = gt->tm_hour;
    pt->pt_min = gt->tm_min;
    pt->pt_sec = gt->tm_sec;
    pt->pt_isdst = gt->tm_isdst;
    pt->pt_gmtoff = gt->tm_gmtoff;
    pt->zone = gt->tm_zone;

    int year, mon, mday;
    int gy = gt->tm_year+1900, gm = gt->tm_mon+1, 
    gd = gt->tm_mday, jdn;

    if (gy > 1582 || (gy == 1582 && gm > 10) || (gy == 1582 && gm == 10 && gd > 14)) 
		jdn = ((1461 * (gy + 4800 + ((gm - 14) / 12))) / 4) + ((367 * (gm - 2 - 12*((gm-14)/12))) / 12) - ((3 * ((gy + 4900 + ((gm - 14) / 12)) / 100)) / 4) + gd - 32075; 
    else 
		jdn = 367*gy - ((7 * (gy + 5001 + ((gm - 9) / 7))) / 4) + ((275 * gm) / 9) + gd + 1729777;

    int dep = jdn - getJdn(475, 1, 1);
    int cyc = dep / 1029983;
    int rem = dep % 1029983;

    int ycyc;

    if (rem == 1029982) {
        ycyc = 2820;
    }
    else {
        int a = rem / 366;
        ycyc = (2134*a+2816*(rem%366)+2815)/1028522 + a + 1;
    }

    year = ycyc + 2820*cyc + 474;

    if (year <= 0) {
        year--;
    }

    double dy = (double)(jdn) - (double)getJdn(year, 1, 1) + 1.0;

    if (dy <= 186) 
        mon = ceil(dy/31.0);
    else 
        mon = ceil((dy-6.0)/30.0);

    mday = jdn - getJdn(year, mon, 1) + 1;

    pt->pt_year = year;
    pt->pt_mon = mon;
    pt->pt_mday = mday;
    pt->pt_yday = calcultateYday(pt);
	
    return pt;
}

ptime *ptimeNow() {
    time_t currTime;
    time(&currTime);
    gtime *gt = localtime(&currTime);
    return convertToJalali(gt);
}

_Bool isLeap(ptime *pt) {
    return isLeapYear(pt->pt_year);
}

ptime *addDays(ptime *pt, int add) {
    gtime *gt = convertToGregourian(pt);
    gt->tm_mday += add;
    mktime(gt);
    return convertToJalali(gt);
}

ptime *addMonths(ptime *pt, int add) {
    gtime *gt = convertToGregourian(pt);
    gt->tm_mon += add;
    mktime(gt);
    return convertToJalali(gt);
}

ptime *addYears(ptime *pt, int add) {
    gtime *gt = convertToGregourian(pt);
    gt->tm_year += add;
    mktime(gt);
    return convertToJalali(gt);
}

char *nameOfDay(int wday) {
    return nameOfDays[wday];
}