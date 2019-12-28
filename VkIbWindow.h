#ifndef VKIBWINDOW_H
#define VKIBWINDOW_H

// Qt
#include <QWindow>
#include <QVulkanInstance>
#include <QLoggingCategory>
#include <qevent.h>

// std
#include <functional>

// ignimbrite
#include <ignimbrite/RenderDevice.h>

namespace ignimbrite {
    class VulkanRenderDevice;
}

class IVkIbApp {
public:
    virtual void drawFrame() = 0;
    virtual void releaseResources() = 0;
};

class VkIbWindow : public QWindow {
public:
    VkIbWindow();
    ~VkIbWindow();

    void initSurface();

    void setVkIbApp(IVkIbApp *app);

    ignimbrite::ObjectID getSurfaceId();
    ignimbrite::RenderDevice *getRenderDevice();

private:
    void initAll();
    void initDevice();

    void destroyAll();
    void destroyDevice();
    void destroySurfaceResources();

    void drawFrame();

    void exposeEvent(QExposeEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    bool event(QEvent *e) override;

private:
    bool isInitialized;
    ignimbrite::VulkanRenderDevice *device;
    ignimbrite::ObjectID surfaceId;

    QVulkanInstance *qvkInstance;

    IVkIbApp *vkIbApp;
};


#endif // VKIBWINDOW_H
