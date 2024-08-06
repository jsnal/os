/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>

__BEGIN_DECLS

int tcgetattr(int fd, struct termios* termios_p)
{
    return ioctl(fd, TCGETS, termios_p);
}

int tcsetattr(int fd, int optional_actions, const struct termios* termios_p)
{
    switch (optional_actions) {
        case TCSANOW:
            return ioctl(fd, TCSETS, termios_p);
        case TCSADRAIN:
            return ioctl(fd, TCSETSW, termios_p);
        case TCSAFLUSH:
            return ioctl(fd, TCSETSF, termios_p);
    }
    errno = EINVAL;
    return -1;
}

__END_DECLS
