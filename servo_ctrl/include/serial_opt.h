
#ifndef _SERIAL_OPT_H_
#define _SERIAL_OPT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define SERIAL_RET_FAILED -1
#define SERIAL_RET_SUCCESS 0

    /**
     * \brief Set data format for serial
     * \param fd: file descriptor for serial port
     * \param speed: serial port speed, e.g., 115200
     * \return SERIAL_RET_FAILED or SERIAL_RET_SUCCESS
     */
    int serial_set_speed(int fd, int speed);

    /**
     * \brief Set data format for serial
     * \param fd: file descriptor for serial port
     * \param databits: data bits for one frame, only 7 and 8 are supported   
     * \param stopbits: stop bits for one frame, only 1 and 2 are supported   
     * \param parity: parity option, 'N' as no parity, 'O' as odd parity, 'E' as even parity and 'S' as stop as parity
     * \return SERIAL_RET_FAILED or SERIAL_RET_SUCCESS
     */
    int serial_set_format(int fd, int databits, int stopbits, char parity);

#ifdef __cplusplus
}
#endif

#endif /* _SERIAL_OPT_H_ */