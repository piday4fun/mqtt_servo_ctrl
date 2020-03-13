
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "serial_opt.h"
#include <sys/select.h>
#include <sys/types.h>

#include "MemeServoAPI.h"


#define MASTER_ADDR 0x01
#define SERVO_ADDR 0x03

#define TIMEOUT_VAL 10000 /* in ms */

int serial_fd = -1;

void send_data_impl(uint8_t addr, uint8_t *data, uint8_t size)
{
    (void)addr; /* unused */

    /* Send data to servo */
    write(serial_fd, data, size);
}

void recv_data_impl()
{
    /* Receive data from servo & parse */
    uint8_t ch;
    int data_got = 0;

    // printf("Try to read data.\n");

    while (read(serial_fd, &ch, 1) > 0)
    {
        data_got++;
        printf("0x%02x ", ch);
        MMS_OnData((uint8_t)ch);
    }

    if (data_got > 0)
    {
        printf("\n");
    }
}

uint32_t get_milliseconds()
{
    struct timespec ts;
    uint32_t ret;

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
    {
        printf("clock_gettime() error\n");
        exit(-1);
    }

    ret = (uint32_t)(ts.tv_sec * 1000UL + ts.tv_nsec / 1e06);

    return ret;
}

void delay_milliseconds(uint32_t ms)
{
    uint32_t tick_start = get_milliseconds();

    while ((uint32_t)(get_milliseconds() - tick_start) < ms)
    {
        usleep(1000);
    }
}

void servo_err_callback(uint8_t node_addr, uint8_t err)
{
    printf("Servo 0x%02x returned error: 0x%02x\n", node_addr, err);
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main()
{
    int ret;
    // struct termios options;
    uint8_t servo_addr;
    fd_set readfds;
    struct timeval tv;
    int fd_stdin;

    uint32_t last_active_time;

    fd_stdin = fileno(stdin);

    /* Open serial port */
    ret = open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);

    if (ret == -1)
    {
        perror("serialport error\n");
        exit(-1);
    }
    else
    {
        serial_fd = ret;
        printf("open %s succesfully\n", ttyname(serial_fd));
    }

    /* Set speed */
    if (serial_set_speed(serial_fd, 115200) != SERIAL_RET_SUCCESS)
    {
        printf("Set speed error\n");
        exit(-1);
    }

    /* Set frame format */
    if (serial_set_format(serial_fd, 8, 1, 'N') != SERIAL_RET_SUCCESS)
    {
        printf("Set parity error\n");
        exit(0);
    }

    /* Initialize servo interface */
    MMS_SetProtocol(MMS_PROTOCOL_UART, MASTER_ADDR, send_data_impl, recv_data_impl);
    MMS_SetTimerFunction(get_milliseconds, delay_milliseconds);
    MMS_SetCommandTimeOut(200);

    /* Start first available servo */

    for (servo_addr = 0x02; servo_addr < 0xFF; servo_addr++)
    {
        printf("Scaning servo 0x%02x\n", servo_addr);

        if ((ret = MMS_StartServo(servo_addr, MMS_MODE_ZERO, servo_err_callback)) != MMS_RESP_SUCCESS)
        {
            // printf("MMS_StartServo(0x%02x) failed, ret=0x%02x\n", servo_addr, ret);
        }
        else
        {
            break;
        }
    }

    if (servo_addr < 0xFF)
    {
        printf("Found servo, addr=0x%02x\n", servo_addr);
    }
    else
    {
        printf("Servo not found, abort.\n");
        exit(-1);
    }

    last_active_time = 0;

    while (1)
    {
        float interval;
        int num_readable;

        FD_ZERO(&readfds);
        FD_SET(fileno(stdin), &readfds);

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        num_readable = select(fd_stdin + 1, &readfds, NULL, NULL, &tv);

        if (num_readable == -1)
        {
            fprintf(stderr, "\nError in select : %s\n", strerror(errno));
            exit(-1);
        }

        if (num_readable == 0)
        {
            printf("Nothing got.\n");

            if ((uint32_t)(get_milliseconds() - last_active_time) > TIMEOUT_VAL)
            {
                static uint32_t used_last_active_time;

                if (last_active_time != used_last_active_time)
                {
                    printf("Timeout when waiting interval value, reset servo.\n");

                    if ((ret = MMS_ProfiledAbsolutePositionMove(servo_addr, 0, servo_err_callback)) != MMS_RESP_SUCCESS)
                    {
                        printf("MMS_ProfiledAbsolutePositionMove failed, ret=0x%02x\n", ret);
                    }

                    used_last_active_time = last_active_time;
                }
            }
        }
        else
        {
            if (fscanf(stdin, "%f", &interval) == 1)
            {
                /* Read full line, process */
                /* Move servo */
                uint32_t pos = (uint32_t)map(interval, 10, 0, 0, 4095);

                printf("interval: %f, pos: %d\n", interval, pos);

                if ((ret = MMS_ProfiledAbsolutePositionMove(servo_addr, pos, servo_err_callback)) != MMS_RESP_SUCCESS)
                {
                    printf("MMS_ProfiledAbsolutePositionMove failed, ret=0x%02x\n", ret);
                }

                last_active_time = get_milliseconds();
            }
            else
            {
                /* line was truncated, discard */
            }
        }
    }

    return 0;
}
