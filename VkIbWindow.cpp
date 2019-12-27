#include "VkIbWindow.h"
#include <VulkanExtensions.h>

using namespace ignimbrite;

VkIbWindow::VkIbWindow() : vkIbApp(nullptr), isInitialized(false)  {
    setSurfaceType(VulkanSurface);
    initDevice();
}

VkIbWindow::~VkIbWindow() {
    destroyAll();
}

ObjectID VkIbWindow::getSurfaceId() {
    return surfaceId;
}

RenderDevice *VkIbWindow::getRenderDevice() {
    return (ignimbrite::RenderDevice*)device;
}

void VkIbWindow::initAll() {
    initDevice();
    initSurface();
}

void VkIbWindow::initDevice() {
    // TODO: correct extensions
    const char *ext[] = {"VK_KHR_surface", "VK_KHR_xcb_surface"};
    device = new VulkanRenderDevice(2, ext);

    // allocate as QWindow stores only a pointer
    qvkInstance = new QVulkanInstance();
    qvkInstance->setLayers(QByteArrayList()
                   << "VK_LAYER_GOOGLE_threading"
                   << "VK_LAYER_LUNARG_parameter_validation"
                   << "VK_LAYER_LUNARG_object_tracker"
                   << "VK_LAYER_LUNARG_core_validation"
                   << "VK_LAYER_LUNARG_image"
                   << "VK_LAYER_LUNARG_swapchain"
                   << "VK_LAYER_GOOGLE_unique_objects");

    // set to QVulkanInstance VkInstance from created render device
    VulkanExtensions::setVulkanInstance(*device, qvkInstance);

    if (!qvkInstance->create()) {
        qFatal("Failed to create Vulkan instance: %d", qvkInstance->errorCode());
    }

    // set QVulkanInstance for this window
    setVulkanInstance(qvkInstance);
}

void VkIbWindow::destroyAll() {
    // firstly, delete surface resources
    destroySurfaceResources();

    // release app resources to delete device
    if (vkIbApp != nullptr) {
        vkIbApp->releaseResources();
    }

    // all associated resources are deleted,
    // device can be deleted
    destroyDevice();
}

void VkIbWindow::initSurface() {
    if (isInitialized) {
        return;
    }

    if (device) {
        // to get surface window must be shown
        surfaceId = VulkanExtensions::createSurfaceQtWidget(*device, this);
        isInitialized = true;

        // surface created, swap for initialization
        device->swapBuffers(surfaceId);

    } else {
        qFatal("Can't create surface: initDevice() must be called before initSurface()");
    }
}

void VkIbWindow::setVkIbApp(IVkIbApp *app) {
    vkIbApp = app;
}

void VkIbWindow::destroyDevice() {
    delete device;

    // QVulkanInstance doesn't own VkInstance
    delete qvkInstance;

    device = nullptr;
    qvkInstance = nullptr;
}

void VkIbWindow::destroySurfaceResources() {
    if (!isInitialized) {
        return;
    }

    // destroy ignimbrite surface, but not VkSurfaceKHR
    ignimbrite::VulkanExtensions::destroySurface(*device, surfaceId, false);

    isInitialized = false;
}

void VkIbWindow::drawFrame() {
    if (vkIbApp != nullptr) {
        vkIbApp->drawFrame();
        device->swapBuffers(surfaceId);
    }

    requestUpdate();
}

void VkIbWindow::exposeEvent(QExposeEvent *) {
    // ensure that window is initilized
    if (isExposed()) {
        if (!isInitialized) {
            qDebug("Exposed but not initialized. Initializing VkIBWindow");
            initAll();
        }

        requestUpdate();

    } else {
        if (isInitialized) {
            qDebug("Unexposed VkIBWindow");
            destroyAll();
        }
    }
}

void VkIbWindow::resizeEvent(QResizeEvent *e) {
    // do nothing, as swapBuffers will resize automatically
}

bool VkIbWindow::event(QEvent *e) {

    switch (e->type()) {

    case QEvent::UpdateRequest:
        drawFrame();
        break;

    case QEvent::PlatformSurface:
        if (static_cast<QPlatformSurfaceEvent *>(e)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
            destroySurfaceResources();
        break;

    default:
        break;
    }

    return QWindow::event(e);
}
