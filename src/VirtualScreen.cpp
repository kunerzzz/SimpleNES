#include <cstdlib>
#include <cstring>

#include "VirtualScreen.h"
#include "Log.h"

namespace sn
{
    void VirtualScreen::create(unsigned int w, unsigned int h, uint32_t color)
    {
        buffer = (uint32_t *)malloc(w * h * sizeof(uint32_t));
        height = h;
        width = w;
        for(int i = 0;i < h * w;++i)
            buffer[i] = color;
    }

    void VirtualScreen::setPixel(unsigned x, unsigned y, uint32_t color)
    {
        if(x < 0 || x >= width || y < 0 || y >= width) {
            LOG(Error) << "setPixel position out of range." << std::endl;
            return;
        }
        buffer[y * width + x] = color;
    }

    uint32_t VirtualScreen::getPixel(unsigned x, unsigned y)
    {
        if(x < 0 || x >= width || y < 0 || y >= width) {
            LOG(Error) << "setPixel position out of range." << std::endl;
            return 0;
        }

        //transform uint32_t color from RGBA to ARGB;
        uint32_t color_b = buffer[y * width + x];
        uint32_t color_l = color_b >> 8;
        color_l |= ((color_b << 24) & 0xff000000);

        return color_l;
    }
}