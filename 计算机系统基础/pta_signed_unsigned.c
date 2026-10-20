#include <stdio.h>

/* PTA：数的有符号值和无符号值（计算机系统基础）
 *
 * 输入：十进制正整数（255、32768、…）或十六进制数（0xaa、0xFFFF、…）
 * 输出：16 位（1 符号位 + 15 数值位）存储值的十六进制、按 short 解释的值、
 *       按 unsigned short 解释的值；数值超出 16 位范围（> 0xFFFF）时输出 -1。
 */

int main(void)
{
    char s[256];

    while (scanf("%255s", s) == 1) {
        const char *p = s;
        int base = 10;

        /* 进制识别：0x / 0X 前缀 -> 十六进制；
           无前缀但出现 a-f / A-F -> 也按十六进制处理（题目给的是 0x 前缀形式，这里兜底） */
        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
            base = 16;
            p = s + 2;
        } else {
            for (const char *q = s; *q; q++) {
                if ((*q >= 'a' && *q <= 'f') || (*q >= 'A' && *q <= 'F')) {
                    base = 16;
                    break;
                }
            }
        }

        unsigned long long x = 0;
        int overflow = 0;

        for (; *p; p++) {
            unsigned d;
            if      (*p >= '0' && *p <= '9') d = (unsigned)(*p - '0');
            else if (*p >= 'a' && *p <= 'f') d = (unsigned)(*p - 'a' + 10);
            else if (*p >= 'A' && *p <= 'F') d = (unsigned)(*p - 'A' + 10);
            else break;                     /* 非法字符，截断 */

            if (!overflow) {
                if (x > (0xFFFFFFFFFFFFFFFFULL - d) / (unsigned)base)
                    overflow = 1;           /* 防 64 位累加溢出 */
                else
                    x = x * (unsigned)base + d;
            }
            if (x > 0xFFFFULL)
                overflow = 1;               /* 超出 16 位可表示范围 */
        }

        if (overflow) {
            printf("-1\n");
            continue;
        }

        /* 16 位存储值，及其两种解释 */
        unsigned short us = (unsigned short)x;
        int signed_val = (us < 0x8000u) ? (int)us : (int)us - 0x10000;

        printf("Hex=%04X, Signed=%d, Unsigned=%u\n",
               (unsigned)us, signed_val, (unsigned)us);
    }

    return 0;
}
