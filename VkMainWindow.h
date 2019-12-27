#ifndef VKMAINWINDOW_H
#define VKMAINWINDOW_H

#include <QApplication>
#include <QVulkanInstance>
#include <QLoggingCategory>
#include <QtCore/QObject>

#include <QLCDNumber>
#include <QPushButton>
#include <QFileDialog>

#include <QWindow>
#include <QGridLayout>
#include <QWidget>

#include <thread>
#include <ios>
#include <functional>

class MainVkWindow : public QWidget {

    Q_OBJECT

public:
    MainVkWindow(QWindow *w) {

        QWidget *wrapper = QWidget::createWindowContainer(w);
        // enable keyboard
        wrapper->setFocusPolicy(Qt::StrongFocus);
        wrapper->setFocus();

        QLCDNumber *counterLcd = new QLCDNumber(5);
        counterLcd->setSegmentStyle(QLCDNumber::Filled);
        counterLcd->display(59);

        QPushButton *loadMeshBtn = new QPushButton("&Load .obj", this);

        connect(loadMeshBtn, &QPushButton::released, this, &MainVkWindow::selectFile);
        //connect(this, &MainVkWindow::destroyed, this, &MainVkWindow::stopVulkanApp);

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(counterLcd, 1, 2);
        layout->addWidget(loadMeshBtn, 2, 2);
        layout->addWidget(wrapper, 0, 0, 7, 2);

        this->setLayout(layout);
    }

    void setMeshLoadingFunction(std::function<void(const char*)> func) {
        loadMesh = func;
    }

public slots:
    void selectFile() {
        QString fileName = QFileDialog::getOpenFileName(
              this,
              tr("Load 3D Model"),
              ".",
              "3D Model (*.obj)");

        if (fileName.endsWith(".obj")) {
            if (loadMesh) {
                loadMesh(fileName.toStdString().c_str());

            } else {
                qWarning("Lambda loadMesh is not set");
            }
        }
    }

private:
    std::function<void(const char*)> loadMesh;
};

#endif // VKMAINWINDOW_H
