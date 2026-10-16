#include <stdio.h>

/* 无符号加法 x+y 的溢出（最高位产生进位）
     1) x、y 最高位都为 1                        -> 溢出
     2) 有一个最高位为 1，且和的最高位不为 1      -> 溢出
   位运算： (MSB(x) & MSB(y)) | ((MSB(x) | MSB(y)) & ~MSB(x+y))           */
static unsigned int uadd_overflow(unsigned int x, unsigned int y)
{
    return ((x & y) | ((x | y) & ~(x + y))) >> 31;
}

/* 无符号减法 x-y 的溢出（借位，即 x < y）
     e = x ^ y      最高位为 1 表示 x、y 最高位不同
     最高位不同：大小由最高位决定 -> 借位 = MSB(~x & y)
     最高位相同：借位 = x < y，此时看回绕差 d = x - y 的最高位
                （x < y 时 d 会回绕成很大的值，最高位与 x 不同）
   合起来：借位 = MSB( (~x & y) | (~e & (x - y)) )                         */
static unsigned int usub_overflow(unsigned int x, unsigned int y)
{
    unsigned int d = x - y;
    return ((~x & y) | (~(x ^ y) & d)) >> 31;
}

int main(void)
{
    unsigned int x, y;
    if (scanf("%u %u", &x, &y) != 2)
        return 0;

    printf("uadd overflow=%u, usub overflow=%u\n",
           uadd_overflow(x, y), usub_overflow(x, y));
    return 0;
}
