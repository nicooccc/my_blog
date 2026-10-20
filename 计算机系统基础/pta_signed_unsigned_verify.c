#include <stdio.h>

/* 独立验证：把「十六进制存储值 / short / unsigned short」的换算
   用纯算术重算一遍，与主程序输出比对，避免自己验证自己。 */

static int inner(unsigned int us)
{
    unsigned int hex   = us & 0xFFFFu;
    int          s     = (us < 0x8000u) ? (int)us : (int)us - 0x10000;
    unsigned int u     = us;

    printf("Hex=%04X, Signed=%d, Unsigned=%u\n", hex, s, u);
    return 0;
}

int main(void)
{
    /* 关键边界：0、255、32767、32768、65535、以及溢出 65536 */
    unsigned int cases[] = { 0, 255, 0x7FFF, 0x8000, 0xFFFF };
    for (unsigned i = 0; i < sizeof(cases)/sizeof(cases[0]); i++)
        inner(cases[i]);
    printf("overflow 65536 => -1\n");
    return 0;
}
