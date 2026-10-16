#include <stdio.h>

int main(void)
{
    unsigned int x;

    /* 十六进制输入，0x 由 %x / %i 自动识别 */
    if (scanf("%x", &x) != 1)
        return 0;

    x |= 0xF0000000u;   /* 第 31~28 位置 1 */
    x &= ~0x00F00000u;  /* 第 23~20 位清 0 */
    x ^= 0x00000FFFu;   /* 第 11~0 位取反（原值低 12 位全部翻转） */

    printf("0x%08x\n", x);
    return 0;
}
