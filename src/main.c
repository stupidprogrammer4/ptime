#include "ptime.h"

int main() {
    ptime *pt = ptimeNow();
    printf("%d %d %d %d", pt->pt_year, pt->pt_mon, pt->pt_mday, pt->pt_wday);
}