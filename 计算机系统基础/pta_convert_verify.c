#include <stdio.h>

/* 独立验证：用"取低 k 位"的纯算术方式重算每个类型应有的十六进制与十进制，
   再与转换结果比对，避免"用同一个错误公式验证自己"。 */

static unsigned long long low_bits(long long v, int k)
{
    /* 数学意义上的 v mod 2^k，对负数也成立（结果落在 [0, 2^k) ） */
    unsigned long long m = 1ULL << k;
    long long r = v % (long long)m;
    if (r < 0) r += (long long)m;
    return (unsigned long long)r;
}

static int check_one(int x)
{
    int bad = 0;

    /* int / unsigned int：低 32 位 */
    unsigned long long b32 = low_bits(x, 32);
    unsigned int vi = (unsigned int)(int)x;
    unsigned int vu = (unsigned int)x;
    if ((unsigned int)((unsigned int)vi & 0xFFFFFFFFu) != (unsigned int)b32) bad++;
    if ((unsigned int)(vu & 0xFFFFFFFFu) != (unsigned int)b32) bad++;
    if ((long long)(int)x != (long long)x) bad++;                 /* 值不变 */
    if ((unsigned long long)vu != b32) bad++;                     /* 无符号值是低 32 位 */

    /* short / unsigned short：低 16 位 */
    unsigned long long b16 = low_bits(x, 16);
    short vs = (short)x;
    unsigned short vus = (unsigned short)x;
    if ((unsigned int)((unsigned int)vs & 0xFFFFu) != (unsigned int)b16) bad++;
    if ((unsigned int)((unsigned int)vus & 0xFFFFu) != (unsigned int)b16) bad++;
    if ((vus & 0xFFFFu) != b16) bad++;
    /* short 的十进制值 = 低 16 位的补码解释 */
    long long signed16 = (b16 < 32768ULL) ? (long long)b16 : (long long)b16 - 65536LL;
    if ((long long)vs != signed16) bad++;

    /* char / unsigned char：低 8 位 */
    unsigned long long b8 = low_bits(x, 8);
    char vc = (char)x;
    unsigned char vuc = (unsigned char)x;
    if ((unsigned int)((unsigned int)vc & 0xFFu) != (unsigned int)b8) bad++;
    if ((unsigned int)((unsigned int)vuc & 0xFFu) != (unsigned int)b8) bad++;
    if ((unsigned int)(vuc & 0xFFu) != b8) bad++;
    long long signed8 = (b8 < 128ULL) ? (long long)b8 : (long long)b8 - 256LL;
    if ((long long)vc != signed8) bad++;                          /* 本机 char 为有符号 char */

    return bad;
}

int main(void)
{
    long long bad = 0, idx = 0;

    /* 全部 65536 个合法输入穷举 */
    for (int x = -32768; x <= 32767; x++) {
        int b = check_one(x);
        if (b) { if (bad < 5) printf("BAD x=%d -> %d 处不符\n", x, b); bad++; }
        idx++;
    }

    /* char 是否有符号：影响第 5 行的十进制值 */
    char c = (char)0xFF;
    printf("本机 char 是否有符号: %s (char)0xFF = %d\n", (c < 0) ? "是（signed char）" : "否（unsigned char）", (int)c);

    /* 关键边界值：逐字节展开便于人工核对 */
    int probes[6] = { -32768, -1, 0, 1, 255, 32767 };
    for (int i = 0; i < 6; i++) {
        int x = probes[i];
        unsigned long long b32 = low_bits(x, 32), b16 = low_bits(x, 16), b8 = low_bits(x, 8);
        short vs = (short)x; char vc = (char)x; unsigned short vus = (unsigned short)x; unsigned char vuc = (unsigned char)x;
        long long s16 = (b16 < 32768ULL) ? (long long)b16 : (long long)b16 - 65536LL;
        long long s8  = (b8  < 128ULL)   ? (long long)b8  : (long long)b8  - 256LL;
        printf("x=%6d | b32=%08llX %08X | b16=%04llX %04X (s=%lld) | b8=%02llX %02X (s=%lld, u=%u)\n",
               x, b32, (unsigned int)x, b16, (unsigned int)vus, s16, b8, (unsigned int)vuc, s8, (unsigned int)vuc);
        (void)vs; (void)vc;
    }

    printf("checked=%lld mismatches=%lld\n", idx, bad);
    return bad ? 1 : 0;
}
