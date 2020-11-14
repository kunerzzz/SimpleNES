#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <unordered_set>
#include <thread>
#include <linux/input-event-codes.h>
#include <mutex>

namespace sn
{
    class InputHandler
    {
    public:
        void create(const char* dev_path);
        bool isKeyPressed(uint16_t keyCode);
        ~InputHandler();
    private:
        std::unordered_set<uint16_t> keyPressed;
        std::thread *pollThread;
        std::mutex mutex;
    };
}

#endif