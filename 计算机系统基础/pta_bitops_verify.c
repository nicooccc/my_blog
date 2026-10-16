#include <stdio.h>

/* ============ 待验证的实现（与 pta_bitops.c 完全一致） ============ */
static unsigned int impl(unsigned int x)
{
    x |= 0xF0000000u;
    x &= ~0x00F00000u;
    x ^= 0x00000FFFu;
    return x;
}

/* ============ 参考实现 1：题目文字直译 ============ */
static unsigned int ref_text(unsigned int x)
{
    unsigned int y = x;
    y = (y & ~0xF0000000u) | 0xF0000000u;
    y = y & ~0x00F00000u;
    y = y ^ 0x00000FFFu;
    return y;
}

/* ============ 参考实现 2：逐位重建，不用十六进制掩码 ============ */
static unsigned int ref_bits(unsigned int x)
{
    unsigned int y = 0;
    for (int b = 31; b >= 0; b--) {
        unsigned int bit;
        if (b >= 28)                     bit = 1;                      /* 31~28 -> 1  */
        else if (b >= 24)                bit = (x >> b) & 1u;          /* 27~24 保持  */
        else if (b >= 20)                bit = 0;                      /* 23~20 -> 0  */
        else if (b >= 12)                bit = (x >> b) & 1u;          /* 19~12 保持  */
        else                             bit = 1u - ((x >> b) & 1u);  /* 11~0 取反   */
        y |= bit << b;
    }
    return y;
}

/* ============ 检查 1：题目三条要求 + 未被要求的位不变 ============ */
/* 未被要求的位 = 27~24 与 19~12；对应掩码 = ~(0xF00FF000 | 0x00000FFF) = 0x0FF00000 */
static int check_conditions(unsigned int x, unsigned int y, const char **why)
{
    static const char *msgs[] = { "ok", "31~28 不全为 1", "23~20 不全为 0", "11~0 未取反",
                                  "27~24 被改动", "19~12 被改动" };
    int code;
    if (((y >> 28) & 0xFu) != 0xFu)                      code = 1;
    else if (((y >> 20) & 0xFu) != 0x0u)                 code = 2;
    else if ((y & 0xFFFu) != ((~x) & 0xFFFu))            code = 3;
    else if (((y >> 24) & 0xFu) != ((x >> 24) & 0xFu))   code = 4;
    else if (((y >> 12) & 0xFFu) != ((x >> 12) & 0xFFu)) code = 5;
    else                                                 code = 0;
    if (why) *why = msgs[code];
    return code;
}

/* ============ 检查 2：逐位对比 ============ */
static int same_bits(unsigned int a, unsigned int b)
{
    for (int k = 0; k < 32; k++)
        if (((a >> k) & 1u) != ((b >> k) & 1u)) return 0;
    return 1;
}

/* ============ 检查 3：直接比较结果中 20 个目标位的取值（最朴素的描述） ============ */
static int check_target_bits(unsigned int x, unsigned int y)
{
    for (int b = 28; b <= 31; b++) if (((y >> b) & 1u) != 1u) return 0;          /* 全 1 */
    for (int b = 20; b <= 23; b++) if (((y >> b) & 1u) != 0u) return 0;          /* 全 0 */
    for (int b = 0;  b <= 11; b++) if (((y >> b) & 1u) == ((x >> b) & 1u)) return 0; /* 取反 */
    return 1;
}

int main(void)
{
    long long bad = 0, idx = 0;
    unsigned int x = 0x243F6A88u;              /* 固定种子 */

    /* A) 3000 万个伪随机输入：生成器是 xorshift，与 impl 独立 */
    for (long long k = 0; k < 30000000LL; k++) {
        x ^= x << 13; x ^= x >> 17; x ^= x << 5;
        unsigned int y = impl(x);
        const char *why = 0;
        if (!same_bits(y, ref_text(x)) || !same_bits(y, ref_bits(x)) ||
            !check_target_bits(x, y) || check_conditions(x, y, &why)) {
            if (bad++ < 5) printf("BAD 0x%08x -> 0x%08x (%s)\n", x, y, why);
        }
        idx++;
    }

    /* B) 穷举低 20 位（1,048,576 个输入） */
    for (unsigned int low = 0; low < 0x100000u; low++) {
        unsigned int y = impl(low);
        const char *why = 0;
        if (!same_bits(y, ref_text(low)) || !same_bits(y, ref_bits(low)) ||
            !check_target_bits(low, y) || check_conditions(low, y, &why)) {
            if (bad++ < 5) printf("EXH BAD 0x%08x -> 0x%08x (%s)\n", low, y, why);
        }
        idx++;
    }

    /* C) 边界：全 0 / 全 1 / 样例 / 各区段单独为 1 */
    unsigned int edge[8] = { 0x00000000u, 0xFFFFFFFFu, 0x12345678u, 0xF0000000u,
                             0x00F00000u, 0x00000FFFu, 0x80000000u, 0x7FFFFFFFu };
    for (int i = 0; i < 8; i++) {
        unsigned int y = impl(edge[i]);
        const char *why = 0;
        int c = check_conditions(edge[i], y, &why);
        if (!same_bits(y, ref_text(edge[i])) || !same_bits(y, ref_bits(edge[i])) ||
            !check_target_bits(edge[i], y) || c) bad++;
        printf("0x%08x -> 0x%08x  ref1=0x%08x ref2=0x%08x  %s\n",
               edge[i], y, ref_text(edge[i]), ref_bits(edge[i]), why);
    }

    /* D) 每个单比特输入 */
    for (int b = 0; b < 32; b++) {
        unsigned int t = 1u << b;
        unsigned int y = impl(t);
        const char *why = 0;
        if (!same_bits(y, ref_text(t)) || !check_target_bits(t, y) || check_conditions(t, y, &why)) {
            if (bad++ < 5) printf("BIT BAD bit%d -> 0x%08x (%s)\n", b, y, why);
        }
        idx++;
    }

    /* E) 特征化：100 万随机输入后，哪些位至少被改过一次 */
    {
        unsigned int flip[32] = {0};
        unsigned int s = 0x9E3779B9u;
        for (long long k = 0; k < 1000000; k++) {
            s ^= s << 13; s ^= s >> 17; s ^= s << 5;
            unsigned int d = s ^ impl(s);
            for (int b = 0; b < 32; b++) if ((d >> b) & 1u) flip[b] = 1;
        }
        printf("被改动过的位 (bit31..bit0): ");
        for (int b = 31; b >= 0; b--) putchar(flip[b] ? '1' : '0');
        printf("\n");
    }

    printf("checks=%lld mismatches=%lld\n", idx + 8, bad);
    return bad ? 1 : 0;
}
