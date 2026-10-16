#include <stdio.h>

int main(void)
{
    int x;

    if (scanf("%d", &x) != 1)
        return 0;

    /* 依次用强制类型转换得到目标类型，再按需打印 */
    int            vi = (int)x;
    unsigned int   vu = (unsigned int)x;
    short          vs = (short)x;
    unsigned short vus = (unsigned short)x;
    char           vc = (char)x;
    unsigned char  vuc = (unsigned char)x;

    /* int / unsigned int：32 位，8 位十六进制。
       把值转成 unsigned int 后取低 32 位，避免负数按 %X 打印时的符号扩展问题 */
    printf("int: %08X, %d\n",
           (unsigned int)((unsigned int)vi & 0xFFFFFFFFu), vi);
    printf("unsigned int: %08X, %u\n",
           (unsigned int)(vu & 0xFFFFFFFFu), vu);

    /* short / unsigned short：16 位，4 位十六进制。
       这两个类型会先整型提升为 int，必须 &0xFFFF 才是真实的 16 位存储值 */
    printf("short: %04X, %d\n",
           (unsigned int)((unsigned int)vs & 0xFFFFu), (int)vs);
    printf("unsigned short: %04X, %u\n",
           (unsigned int)((unsigned int)vus & 0xFFFFu), (unsigned int)vus);

    /* char / unsigned char：8 位，2 位十六进制。同样先提升为 int，必须 &0xFF */
    printf("char: %02X, %d\n",
           (unsigned int)((unsigned int)vc & 0xFFu), (int)vc);
    printf("unsigned char: %02X, %u\n",
           (unsigned int)((unsigned int)vuc & 0xFFu), (unsigned int)vuc);

    return 0;
}
