#include <QApplication>
#include <QVulkanInstance>
#include <QLoggingCategory>
#include <QtCore/QObject>

#include <QLCDNumber>
#include <QPushButton>
#include <QFileDialog>

#include <QVulkanWindow>
#include <QGridLayout>
#include <QWidget>

#include <thread>
#include <ios>
#include <functional>

#include "VkMainWindow.h"

#include "VkIbWindow.h"
#include "Vk3DRenderer.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    uint32 width = 1024;
    uint32 height = 768;

    VkIbWindow *vulkanWindow = new VkIbWindow();
    vulkanWindow->resize(width, height);

    MainVkWindow mainWindow(vulkanWindow);
    mainWindow.resize(1024, 768);
    mainWindow.show();

    // main window is shown, init VkIbWindow
    vulkanWindow->initSurface();

    Vulkan3DTest application("assets/textures/double.png",
                             vulkanWindow->getRenderDevice(), vulkanWindow->getSurfaceId(),
                             vulkanWindow->width(), vulkanWindow->height());

    vulkanWindow->setVkIbApp(&application);

    mainWindow.setMeshLoadingFunction([&application](const char *path){application.loadObjModel(path);});

    return app.exec();
}


