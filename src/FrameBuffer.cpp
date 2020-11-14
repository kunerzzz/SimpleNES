#include "FrameBuffer.h"
#include "Log.h"

#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <cstdlib>
#include <vector>

namespace sn
{
    void FrameBuffer::create(int w, int h) {
        char default_fbpath[] = "/dev/fb0";

        this->originalWidth = w;
        this->originalHeight = h;

        char *fbpath = getenv("FRAMEBUFFER");
        if(!fbpath)
            fbpath = default_fbpath;

        this->fbdev = open(fbpath, O_RDWR);
        if(this->fbdev < 0) {
            LOG(Error) << "Can not open fb device " << fbpath << "." << std::endl;
            return;
        }

        struct fb_var_screeninfo vinfo;
        if(ioctl(fbdev, FBIOGET_VSCREENINFO, &vinfo)) {
            LOG(Error) << "Can not read vinfo." << std::endl;
            return;
        }

        if(vinfo.bits_per_pixel != 32) {
            LOG(Error) << "Can not handle bits_per_pixel " << vinfo.bits_per_pixel << std::endl;
            return;
        }

        int console_fd = open("/dev/console", O_RDWR);   
        if(ioctl(console_fd, KDSETMODE, KD_GRAPHICS)) {
            LOG(Error) << "Can not set console mode to graphic" << errno << std::endl;
            return;
        }
        close(console_fd);

        fbWidth = vinfo.xres;
        fbHeight = vinfo.yres;
        fbVWidth = vinfo.xres_virtual;
        fbVHeight = vinfo.yres_virtual;
        fbOffsetX = vinfo.xoffset;
        fbOffsetY = vinfo.yoffset;

        float xratio = float(fbWidth) / originalWidth;
        float yratio = float(fbHeight) / originalHeight;
        if(xratio > yratio) {
            scaledHeight = fbHeight;
            scaledWidth = originalWidth * yratio;
            if(scaledWidth > fbWidth)
                scaledWidth = fbWidth;
            
            scaledYMargin = 0;
            scaledXMargin = (fbWidth - scaledWidth) / 2;
        } else {
            scaledWidth = fbWidth;
            scaledHeight = originalHeight * xratio;
            if(scaledHeight > fbHeight)
                scaledHeight = fbHeight;
            
            scaledXMargin = 0;
            scaledYMargin = (fbHeight - scaledHeight) / 2;
        }

        fbmem_size = fbVWidth * fbVHeight * 4;
        fbmem = (uint32_t *)mmap(0, fbmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbdev, 0);
        fbmem_off = (uint32_t *)malloc(fbmem_size);

        LOG(Info) << "Init Frame Buffer: \n";
        LOG(Info) << "Res: " << fbWidth << "x" << fbHeight << std::endl;
        LOG(Info) << "VRes: " << fbVWidth << "x" << fbVHeight << std::endl;
        LOG(Info) << "Original Image size: " << originalWidth << "x" << originalHeight << std::endl;
        LOG(Info) << "Scaled Image size: " << scaledWidth << "x" << scaledHeight << std::endl;   
    }

    void FrameBuffer::draw(VirtualScreen &m_emulatorScreen) {
        for(unsigned y = 0;y < scaledHeight;++y)
            for(unsigned x = 0;x < scaledWidth;++x) {
                unsigned fx = x + scaledXMargin;
                unsigned fy = y + scaledYMargin;
                fbmem_off[getVMemOffset(fx, fy)] = getOrginalPixel(m_emulatorScreen, x, y);
            }
    }

    void FrameBuffer::display() {
        memcpy(fbmem, fbmem_off, fbmem_size);
    }

    FrameBuffer::~FrameBuffer() {
        int console_fd = open("/dev/console", O_RDWR);   
        if(ioctl(console_fd, KDSETMODE, KD_TEXT)) {
            LOG(Error) << "Can not set console mode to text " << errno << std::endl;
        }
        close(console_fd);
        // LOG(Info) << "Delete FrameBuffer" << std::endl;
    }
}