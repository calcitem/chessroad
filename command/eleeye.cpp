/*
eleeye.cpp - Source Code for ElephantEye, Part IX

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdarg.h>

#include "base2.h"
#include "command-channel.h"

const int INTERRUPT_COUNT = 4096; // 搜索若干结点后调用中断

void PrintLn(const char *sz, ...) {

    va_list args;

    va_start(args, sz);

    char buffer[256] = {0};
    vsprintf(buffer, sz, args);

    va_end(args);

    CommandChannel *channel = CommandChannel::getInstance();
    while (!channel->pushResponse(buffer)) Idle();
}

int engineMain(void) {
  PrintLn("bye");
  return 0;
}
