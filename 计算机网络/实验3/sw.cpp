#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_FRAMES 5
#define TIMEOUT 3

int sendframe(int frame_id, int data_id)
{
    printf("发送方:发送帧%d(编号%d)\n", data_id, frame_id);
    return frame_id;
}

/* 接收方独立维护期望序号：收到新帧立即推进，收到旧帧则重发 ACK */
int receiveframe(int frame_id, int *expected_id)
{
    int success = rand() % 2;
    if (!success) {
        printf("接收方：帧编号%d丢失，未发送ACK\n", frame_id);
        return 0;                       /* 帧丢失 */
    }
    if (frame_id == *expected_id) {
        printf("接收方：接受成功帧编号%d，发送ACK序号为%d\n", frame_id, frame_id);
        *expected_id = 1 - *expected_id;      /* 立刻推进期望序号 */
        return 1;                            /* 正常接收 */
    } else {
        printf("接收方：接受重复帧编号%d，并重发ACK%d\n", frame_id, 1 - *expected_id);
        return 2;                            /* 重复帧 */
    }
}

int wait_ack(int frame_id)
{
    int received = rand() % 2;
    if (received) {
        printf("发送方：收到ACK%d，发送下一个\n\n", frame_id);
        return 1;
    }
    printf("发送方：ACK%d超时未收到，准备重传帧编号%d\n\n", frame_id, frame_id);
    return 0;
}

int main()
{
    srand((unsigned)time(NULL));
    int frame_id = 0, expected_id = 0, data_id = 0, res = 0;
    printf("=====SW模拟======\n");
    while (data_id < MAX_FRAMES) {
        sendframe(frame_id, data_id);
        int r = receiveframe(frame_id, &expected_id);

        if (r == 0) {
            /* 帧没到，接收方不会发 ACK，发送方只能超时重传 */
            printf("发送方：未收到ACK，准备重传帧%d\n\n", frame_id);
            res++;
        } else {
            /* 接收方已发出 ACK（新 ACK 或重发的 ACK），等待其是否到达 */
            if (wait_ack(frame_id)) {
                data_id++;                 /* 数据交付上层 */
                frame_id = 1 - frame_id;   /* 进入下一帧 */
                res = 0;
            } else {
                res++;                     /* ACK 丢失，重传同一帧 */
            }
        }

        if (res >= TIMEOUT) {
            printf("发送方：超时过多，停止发送\n");
            break;
        }
    }
    return 0;
}