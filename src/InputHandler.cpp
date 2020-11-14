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
    void pollFunction(int fd, std::unordered_set<uint16_t> *set, std::mutex *mutex) {
        struct input_event event;

        LOG(Info) << "Poll thread started" << std::endl;

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

    std::unordered_map<std::string, InputHandler*> InputHandler::inputHandlerMap = std::unordered_map<std::string, InputHandler*>();

    InputHandler::InputHandler(const char *dev_path) {
        ref_cnt = 0;
        fd = -1;
        if((fd = open(dev_path, O_RDONLY)) < 0) {
            LOG(Error) << "Can not open input device " << dev_path << std::endl;
            return;
        }
        if(ioctl(fd, EVIOCGRAB, 1)) {
            LOG(Error) << "Can not grab input device " << dev_path << std::endl;
        }
        pollThread = new std::thread(pollFunction, fd, &keyPressed, &mutex);
        pollThread->detach();
        LOG(Info) << "InputHandler(dev_path=" << dev_path << ") begins to work" << std::endl;
    }

    void InputHandler::bind() {
        ref_cnt++;
    }

    void InputHandler::unbind() {
        if(--ref_cnt <= 0) {
            LOG(Info) << "Delete InputHandler" << std::endl;
            delete this;
        }
    }

    bool InputHandler::isKeyPressed(uint16_t code) {
        mutex.lock();
        bool result = keyPressed.count(code);
        mutex.unlock();
        return result;
    }

    InputHandler::~InputHandler() {
        if(fd < 0)
            return;
        if(ioctl(fd, EVIOCGRAB, 0)) {
            LOG(Error) << "Can not release input device" << std::endl;
        }
        LOG(Info) << "Kill poll thread" << std::endl;
        delete pollThread;
    }

    InputHandler* InputHandler::Get(std::string dev_path) {
        if(inputHandlerMap[dev_path])
            return inputHandlerMap[dev_path];
        InputHandler *newHandler = new InputHandler(dev_path.c_str());
        inputHandlerMap[dev_path] = newHandler;
        return newHandler;    
    }
}