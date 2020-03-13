
#include "serial_opt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

static int _serial_baudrates[] = {
    50,
    75,
    110,
    134,
    150,
    200,
    300,
    600,
    1200,
    1800,
    2400,
    4800,
    9600,
    19200,
    38400,
    57600,
    115200,
    230400,
    460800,
    500000,
    576000,
    921600};

static speed_t _serial_speeds[] = {
    B50,
    B75,
    B110,
    B134,
    B150,
    B200,
    B300,
    B600,
    B1200,
    B1800,
    B2400,
    B4800,
    B9600,
    B19200,
    B38400,
    B57600,
    B115200,
    B230400,
    B460800,
    B500000,
    B576000,
    B921600};

int serial_set_speed(int fd, int speed)
{
    int i;
    int status;
    struct termios Opt;

    tcgetattr(fd, &Opt);

    for (i = 0; i < sizeof(_serial_baudrates) / sizeof(_serial_baudrates[0]); i++)
    {
        // printf("speed: %d, _serial_baudrates[%d]: %d\n", speed, i, _serial_baudrates[i]);

        if (speed == _serial_baudrates[i])
        {
            // printf("serial speed matched.\n");
            break;
        }
    }

    

    if (i < sizeof(_serial_baudrates) / sizeof(_serial_baudrates[0]))
    {
        tcflush(fd, TCIOFLUSH);
        cfsetispeed(&Opt, _serial_speeds[i]);
        cfsetospeed(&Opt, _serial_speeds[i]);
        status = tcsetattr(fd, TCSANOW, &Opt);

        if (status != 0)
        {
            perror("tcsetattr fd1");
            return SERIAL_RET_FAILED;
        }

        tcflush(fd, TCIOFLUSH);

        return SERIAL_RET_SUCCESS;
    }

    return SERIAL_RET_FAILED;
}

int serial_set_format(int fd, int databits, int stopbits, char parity)
{
    struct termios options;

    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1");
        return (SERIAL_RET_FAILED);
    }

    options.c_cflag &= ~CSIZE;

    switch (databits)
    {
    case 5:
        options.c_cflag |= CS5;
        break;
    case 6:
        options.c_cflag |= CS6;
        break;
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return (SERIAL_RET_FAILED);
    }

    switch (parity)
    {
    case 'n':
    case 'N':
        /* No parity */
        options.c_cflag &= ~PARENB; /* Clear parity enable */
        options.c_iflag &= ~INPCK;  /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        /* Odd parity */
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK; /* Disable parity checking */
        break;
    case 'e':
    case 'E':
        /* Even parity */
        options.c_cflag |= PARENB; /* Enable parity */
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK; /* Disable parity checking */
        break;
    case 'S':
    case 's':
        /* Stop as parity? */
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return (SERIAL_RET_FAILED);
    }

    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return (SERIAL_RET_FAILED);
    }

    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;

    tcflush(fd, TCIFLUSH);
    options.c_cc[VTIME] = 10; /* specifies the time to wait until the characters is received. [VTIME] = 0 to wait indefinitely */
    options.c_cc[VMIN] = 0;    /* specifies the minimum number of characters that should be read before the read() call returns*/

    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("SetupSerial 3");
        return (SERIAL_RET_FAILED);
    }

    return (SERIAL_RET_SUCCESS);
}
