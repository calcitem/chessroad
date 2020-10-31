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
