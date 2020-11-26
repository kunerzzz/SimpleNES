#include <cstdlib>
#include <cstring>

#include "VirtualScreen.h"
#include "Log.h"

namespace sn
{
    void VirtualScreen::create(unsigned int w, unsigned int h, uint32_t color)
    {
        wbuffer = (uint32_t *)malloc(w * h * sizeof(uint32_t));
        rbuffer = (uint32_t *)malloc(w * h * sizeof(uint32_t));
        height = h;
        width = w;
        for(int i = 0;i < h * w;++i) 
            wbuffer[i] = color;
        finish();
    }

    void VirtualScreen::setPixel(unsigned x, unsigned y, uint32_t color)
    {
        if(x >= width || y >= width) {
            LOG(Error) << "setPixel position out of range." << std::endl;
            return;
        }

        //transform uint32_t color from RGBA to ARGB;
        uint32_t color_l = color >> 8;
        color_l |= ((color << 24) & 0xff000000);

        wbuffer[y * width + x] = color_l;
    }

    uint32_t VirtualScreen::getPixel(unsigned x, unsigned y)
    {
        if(x >= width || y >= width) {
            LOG(Error) << "setPixel position out of range." << std::endl;
            return 0;
        }

        return rbuffer[y * width + x];
    }

    void VirtualScreen::finish() {
        memcpy(rbuffer, wbuffer, width * height * sizeof(uint32_t));
    }
}