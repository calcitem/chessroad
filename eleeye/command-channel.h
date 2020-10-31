//
//  command-channel.h
//  Runner
//

#ifndef command_channel_h
#define command_channel_h

class CommandQueue;

class CommandChannel {

    CommandChannel();

public:
    static CommandChannel *getInstance();
    static void release();
    
    virtual ~CommandChannel();

    bool pushCommand(const char *cmd);
    bool popupCommand(char *buffer);
    bool pushResponse(const char *resp);
    bool popupResponse(char *buffer);

private:
    static CommandChannel *instance;

    CommandQueue *commandQueue;
    CommandQueue *responseQueue;
};

#endif /* command_channel_h */
