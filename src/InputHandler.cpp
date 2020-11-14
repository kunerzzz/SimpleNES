#include "InputHandler.h"
#include "Log.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

namespace sn
{
    void pollFunction(const char *path, std::unordered_set<uint16_t> *set, std::mutex *mutex) {
        struct input_event event;
        int fd;

        if((fd = open(path, O_RDONLY)) < 0) {
            LOG(Error) << "Can not open input event file " << path << std::endl;
            return;
        }

        while(1) {
            if(read(fd, &event, sizeof(event)) == sizeof(event)) {
                if(event.type == EV_KEY) {
                    mutex->lock();
                    if(event.value) {
                        set->insert(event.code);
                    } else {
                        set->erase(event.code);
                    }
                    mutex->unlock();
                }
            } else {
                LOG(Error) << "bad read \n";
            }
        }
    }

    void InputHandler::create(const char *dev_path) {
        pollThread = new std::thread(pollFunction, dev_path, &keyPressed, &mutex);
        pollThread->detach();
    }

    bool InputHandler::isKeyPressed(uint16_t code) {
        mutex.lock();
        bool result = keyPressed.count(code);
        mutex.unlock();
        return result;
    }

    InputHandler::~InputHandler() {
        LOG(Info) << "Kill poll thread" << std::endl;
        delete pollThread;
    }
}