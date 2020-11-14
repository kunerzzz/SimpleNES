#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <linux/input-event-codes.h>
#include <mutex>
#include <string>

namespace sn
{
    class InputHandler
    {
    public:
        InputHandler(const char *dev_path);
        void bind();
        void unbind();
        bool isKeyPressed(uint16_t keyCode);
        ~InputHandler();

        static InputHandler* Get(std::string dev_path);
    private:
        int fd;
        int ref_cnt;
        std::unordered_set<uint16_t> keyPressed;
        std::thread *pollThread;
        std::mutex mutex;

        static std::unordered_map<std::string, InputHandler*> inputHandlerMap;
    };
}

#endif