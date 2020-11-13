#ifndef VIRTUALSCREEN_H
#define VIRTUALSCREEN_H

#include <stdint.h>

namespace sn
{
    class VirtualScreen
    {
    public:
        void create (unsigned int width, unsigned int height, uint32_t color);
        void setPixel (unsigned x, unsigned y, uint32_t color);
        uint32_t getPixel (unsigned x, unsigned y);

    private:
        unsigned height;
        unsigned width;
        uint32_t *buffer;
    };
};

#endif // VIRTUALSCREEN_H
