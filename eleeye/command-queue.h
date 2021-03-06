//
//  command-queue.h
//  Runner
//
//  Created by 贺照云 on 2020/3/18.
//

#ifndef command_queue_h
#define command_queue_h

#include <mutex>

class CommandQueue {
    
    enum {
        MAX_COMMAND_COUNT = 128,
        COMMAND_LENGTH = 2048,
    };
    
    char commands[MAX_COMMAND_COUNT][COMMAND_LENGTH];
    int readIndex, writeIndex;

    std::mutex mutex;
    
public:
    CommandQueue();
    
    bool write(const char *command);
    bool read(char *dest);
};

#endif /* command_queue_h */
