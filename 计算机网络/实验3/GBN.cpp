#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_FRAMES 10
#define WINDOW_SIZE 8
#define LOSS_RATE 30
#define TIMEOUT_LIMIT 3

int is_lost() { return (rand() % 100) < LOSS_RATE; }

void send_frame(int id) {
    printf("发送方：发送帧 %d\n", id);
}

int receive_frame(int id, int expected) {
    if (is_lost()) {
        printf("接收方：帧 %d 丢失！未发送ACK。\n", id);
        return 0;
    }
    if (id == expected) {
        printf("接收方：成功接收帧 %d，发送 ACK%d\n", id, id);
        return 1;
    } else {
        printf("接收方：收到非期望帧 %d（期望 %d），丢弃！\n", id, expected);
        return 0;
    }
}

int ack_received() { return !is_lost(); }

int main() {
    srand(time(NULL));
    int base = 0, next_seq = 0, timeout_count = 0;

    printf("=== 滑动窗口协议（Go-Back-N ARQ）模拟 ===\n\n");

    while (base < TOTAL_FRAMES) {
        while (next_seq < base + WINDOW_SIZE && next_seq < TOTAL_FRAMES) {
            send_frame(next_seq);
            next_seq++;
        }

        int success = receive_frame(base, base);
        if (success) {
            if (ack_received()) {
                printf("发送方：收到 ACK%d，窗口前移。\n\n", base);
                base++;
                timeout_count = 0;
            } else {
                printf("发送方：ACK%d 丢失，等待超时重传。\n\n", base);
                timeout_count++;
            }
        } else {
            printf("发送方：未收到正确ACK，准备重传窗口内所有帧。\n\n");
            timeout_count++;
        }

        if (timeout_count >= TIMEOUT_LIMIT) {
            printf("发送方：超时，重传窗口 [%d - %d]\n", base, next_seq - 1);
            for (int i = base; i < next_seq; i++) send_frame(i);
            timeout_count = 0;
        }
    }

    printf("=== 所有帧传输完成 ===\n");
    return 0;
}
