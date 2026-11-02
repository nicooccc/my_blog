#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_FRAMES 8     // 总发送帧数
#define WINDOW_SIZE 4      // 窗口大小
#define LOSS_RATE 20       // 模拟 20% 的丢包率

// 模拟网络丢包函数
int is_lost() { return (rand() % 100) < LOSS_RATE; }

int main() {
    srand((unsigned)time(NULL));
    int base = 0, next_seq = 0;
    int acked[TOTAL_FRAMES] = {0};      // 发送方：记录哪些帧已被确认
    int rcv_base = 0;
    int rcv_buffer[TOTAL_FRAMES] = {0}; // 接收方：缓存乱序到达的帧

    printf("=== 选择重传 (Selective Repeat) 协议模拟 ===\n\n");

    while (base < TOTAL_FRAMES) {
        // 1. 发送方：连续发送窗口内的新帧
        while (next_seq < base + WINDOW_SIZE && next_seq < TOTAL_FRAMES) {
            printf("发送方：发送帧 %d\n", next_seq);
            if (!is_lost()) {
                // 接收方处理
                if (next_seq == rcv_base) {
                    printf("接收方：成功接收帧 %d，按序交付\n", next_seq);
                    rcv_buffer[next_seq] = 1;
                    rcv_base++;
                    // 检查是否有缓存的连续帧可以一并交付
                    while (rcv_buffer[rcv_base] == 1) {
                        printf("接收方：交付缓存的连续帧 %d\n", rcv_base);
                        rcv_base++;
                    }
                    if (!is_lost()) { // ACK 返回成功
                        printf("发送方：收到 ACK %d\n", next_seq);
                        acked[next_seq] = 1;
                    } else printf("发送方：ACK %d 丢失\n", next_seq);
                } 
                else if (next_seq > rcv_base) {
                    printf("接收方：收到乱序帧 %d，缓存起来（不交付）\n", next_seq);
                    rcv_buffer[next_seq] = 1;
                    if (!is_lost()) { // ACK 返回成功
                        printf("发送方：收到 ACK %d\n", next_seq);
                        acked[next_seq] = 1;
                    } else printf("发送方：ACK %d 丢失\n", next_seq);
                }
            } else {
                printf(">>> 网络：帧 %d 丢失！<<<\n", next_seq);
            }
            next_seq++;
        }

        // 2. 滑动窗口：将基序号向前移动到第一个未确认的帧
        while (base < TOTAL_FRAMES && acked[base] == 1) {
            base++;
        }

        // 3. 超时重传处理（模拟定时器）
        int timeout_flag = 0;
        for (int i = base; i < next_seq; i++) {
            if (acked[i] == 0) {
                printf("发送方：帧 %d 超时，仅重传该帧\n", i);
                if (!is_lost()) {
                    if (i == rcv_base) {
                        printf("接收方：成功接收重传帧 %d，交付\n", i);
                        rcv_buffer[i] = 1;
                        rcv_base++;
                        while (rcv_buffer[rcv_base] == 1) {
                            printf("接收方：交付缓存的连续帧 %d\n", rcv_base);
                            rcv_base++;
                        }
                    } else {
                        printf("接收方：收到重复帧 %d，发送 ACK\n", i);
                    }
                    if (!is_lost()) {
                        printf("发送方：收到 ACK %d\n", i);
                        acked[i] = 1;
                    } else printf("发送方：ACK %d 丢失\n", i);
                } else {
                    printf(">>> 网络：重传帧 %d 再次丢失！<<<\n", i);
                }
                timeout_flag = 1;
                break; // 每次只重传一个超时帧
            }
        }

        // 如果全部确认完毕，跳出循环
        if (timeout_flag == 0 && next_seq >= TOTAL_FRAMES) {
            while (base < TOTAL_FRAMES && acked[base] == 1) base++;
            if (base >= TOTAL_FRAMES) break;
        }
    }

    printf("\n=== 所有帧传输完成 ===\n");
    return 0;
}