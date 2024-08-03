/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <termios.h>

__BEGIN_DECLS

int tcgetattr(int fd, struct termios* termios_p)
{
    return 0;
}

__END_DECLS
