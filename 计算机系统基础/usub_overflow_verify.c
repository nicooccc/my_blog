#include <stdio.h>

/* 与 pta_unsigned_overflow.c 完全相同的两个函数 */
static unsigned int uadd_overflow(unsigned int x, unsigned int y)
{
    return ((x & y) | ((x | y) & ~(x + y))) >> 31;
}
static unsigned int usub_overflow(unsigned int x, unsigned int y)
{
    unsigned int d = x - y;
    return ((~x & y) | (~(x ^ y) & d)) >> 31;
}

int main(void)
{
    long long bad = 0;

    /* A) 穷举 x、y 各 16 位：2^32 组 */
    for (unsigned int x = 0; x < 0x10000u; x++)
        for (unsigned int y = 0; y < 0x10000u; y++) {
            if (uadd_overflow(x,y) != (unsigned)((((unsigned long long)x+y)>>32)?1:0)) {
                if (bad<5) printf("UADD BAD %u %u\n", x, y);
                bad++;
            }
            if (usub_overflow(x,y) != (unsigned)(x<y)) {
                if (bad<5) printf("USUB BAD x=%u y=%u got=%u want=%u\n", x, y, usub_overflow(x,y), (unsigned)(x<y));
                bad++;
            }
        }
    printf("A) 穷举 2^32 = 4294967296 组: 不一致 = %lld\n", bad);

    /* B) 随机 32 位全范围 */
    long long bad2 = 0;
    unsigned int p = 0x243F6A88u, q = 0x85EBCA6Bu;
    for (long long k = 0; k < 50000000LL; k++) {
        p ^= p << 13; p ^= p >> 17; p ^= p << 5;
        q ^= q << 13; q ^= q >> 17; q ^= q << 5;
        if (uadd_overflow(p,q) != (unsigned)((p+q)<p)) bad2++;
        if (usub_overflow(p,q) != (unsigned)(p<q))     bad2++;
    }
    printf("B) 随机 32 位 50000000 组: 不一致 = %lld\n", bad2);

    /* C) 样例 */
    printf("\nC) 样例 2147483647 4294967295 -> uadd overflow=%u, usub overflow=%u (期望 1 1) %s\n",
           uadd_overflow(2147483647u,4294967295u), usub_overflow(2147483647u,4294967295u),
           (uadd_overflow(2147483647u,4294967295u)==1 && usub_overflow(2147483647u,4294967295u)==1) ? "通过" : "失败");

    /* D) 边界 */
    unsigned int t[12][2] = {
        {0u,0u},{1u,1u},{4294967295u,4294967295u},{2147483648u,2147483648u},
        {2147483647u,2147483647u},{2147483648u,2147483647u},{2147483647u,2147483648u},
        {1u,0u},{0u,1u},{4294967295u,1u},{0u,4294967295u},{2147483647u,4294967295u}
    };
    printf("\nD) 边界:\n");
    for (int i = 0; i < 12; i++) {
        unsigned int x = t[i][0], y = t[i][1];
        printf("   %-10u %-10u -> uadd=%u usub=%u | 参考 %u %u  %s\n", x, y,
               uadd_overflow(x,y), usub_overflow(x,y),
               (unsigned)((x+y)<x), (unsigned)(x<y),
               (uadd_overflow(x,y)==(unsigned)((x+y)<x) && usub_overflow(x,y)==(unsigned)(x<y)) ? "ok" : "FAIL");
    }
    return (bad+bad2)?1:0;
}
