#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

typedef struct tm gtime;

typedef struct PTIME_ {
    int pt_year;
    int pt_mon;
    int pt_mday;
    int pt_wday;
    int pt_yday;
    int pt_hour;
    int pt_min;
    int pt_sec;
    int pt_isdst;
    long pt_gmtoff;
    const char *zone;
} ptime;

gtime *convertToGregourian(ptime *pt);
ptime *convertToJalali(gtime *);
ptime *ptimeNow();
_Bool isLeap(ptime *);
char *nameOfDay(int);
ptime *addDays(ptime *, int);
ptime *addMonths(ptime *, int);
ptime *addYears(ptime *, int);