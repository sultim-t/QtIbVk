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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VkIbWindow.h"

class TrackableVkIbWindow : public VkIbWindow {
    Q_OBJECT

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        emit notifyParentMouseMove(event);
    }
    void mousePressEvent(QMouseEvent *event) override {
        emit notifyParentMousePress(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        emit notifyParentMouseRelease(event);
    }
    void wheelEvent(QWheelEvent *event) override {
        emit notifyParentWheelEvent(event);
    }
    void keyPressEvent(QKeyEvent *event) override {
        emit notifyParentKeyPressEvent(event);
    }
    void keyReleaseEvent(QKeyEvent *event) override {
        emit notifyParentKeyReleaseEvent(event);
    }

signals:
    void notifyParentMouseMove(QMouseEvent* event);
    void notifyParentMousePress(QMouseEvent* event);
    void notifyParentMouseRelease(QMouseEvent* event);
    void notifyParentWheelEvent(QWheelEvent * event );
    void notifyParentKeyPressEvent(QKeyEvent * event );
    void notifyParentKeyReleaseEvent(QKeyEvent * event );
};

class MainVkWindow : public QWidget {

    Q_OBJECT

public:
    MainVkWindow(TrackableVkIbWindow *w) {

        QWidget *wrapper = QWidget::createWindowContainer(w);
        // enable keyboard and mouse
        wrapper->setFocusPolicy(Qt::StrongFocus);
        wrapper->setFocus();
        wrapper->setMouseTracking(true);

//        QLCDNumber *counterLcd = new QLCDNumber(5);
//        counterLcd->setSegmentStyle(QLCDNumber::Filled);
//        counterLcd->display(59);

        QPushButton *loadMeshBtn = new QPushButton("&Load .obj", this);

        connect(loadMeshBtn, &QPushButton::released, this, &MainVkWindow::selectFile);

        connect(w, &TrackableVkIbWindow::notifyParentMouseMove, this, &MainVkWindow::childMoveEvent);
        connect(w, &TrackableVkIbWindow::notifyParentMousePress, this, &MainVkWindow::childPressEvent);
        connect(w, &TrackableVkIbWindow::notifyParentMouseRelease, this, &MainVkWindow::childReleaseEvent);
        connect(w, &TrackableVkIbWindow::notifyParentWheelEvent, this, &MainVkWindow::childWheelEvent);
        connect(w, &TrackableVkIbWindow::notifyParentKeyPressEvent, this, &MainVkWindow::childKeyPressEvent);
        connect(w, &TrackableVkIbWindow::notifyParentKeyReleaseEvent, this, &MainVkWindow::childKeyReleaseEvent);

        QGridLayout *layout = new QGridLayout;
//        layout->addWidget(counterLcd, 1, 2);
        layout->addWidget(loadMeshBtn, 2, 2);
        layout->addWidget(wrapper, 0, 0, 7, 2);

        this->setLayout(layout);

        vkWidget = wrapper;
    }

    void setMeshLoadingFunction(std::function<void(const char*)> func) {
        loadMesh = func;
    }

    void setMvpUpdateFunction(std::function<void(float *mvpMat44, float *modelMat44)> func) {
        updateMvp = func;
    }

private:
    void updateMatrices() {
        auto projection = glm::perspective(fov, (float)vkWidget->width() / vkWidget->height(), 0.1f, 300.0f);

        auto view = glm::lookAt(
                glm::vec3(0, 0, z),
                glm::vec3(0, 0, 0),
                glm::vec3(0, 1, 0)
        );

        auto model = glm::mat4(1.0f);
        model = glm::rotate(model, pitch, glm::vec3(1, 0, 0));
        model = glm::rotate(model, yaw, glm::vec3(0, 1, 0));

        auto clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.5f, 0.0f,
                              0.0f, 0.0f, 0.5f, 1.0f);

        auto mvp = clip * projection * view * model;

        updateMvp(&mvp[0][0], &model[0][0]);
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

        updateMatrices();
    }

    void childMoveEvent(QMouseEvent *event) {
        const float sensitivity = 1.0f / 400.0f;

        float x = event->globalX();
        float y = event->globalY();

        if (event->buttons() & Qt::LeftButton) {

            yaw += x * sensitivity - prevx;
            pitch -= y * sensitivity - prevy;

            updateMatrices();
        }

        prevx = x * sensitivity;
        prevy = y * sensitivity;
    }

    void childPressEvent(QMouseEvent *event) {

    }

    void childReleaseEvent(QMouseEvent *event) {

    }

    void childWheelEvent(QWheelEvent *event) {
        const float sensitivity = 0.05f;
        z += event->angleDelta().y() * sensitivity;
        updateMatrices();
    }

    void childKeyPressEvent(QKeyEvent *event) {

    }

    void childKeyReleaseEvent(QKeyEvent *event) {

    }

private:
    QWidget *vkWidget;

    std::function<void(const char*)> loadMesh;
    std::function<void(float *mvpMat44, float *modelMat44)> updateMvp;

    float pitch = 0;
    float yaw = 0;
    float fov = 70;
    float z = -80;
    float prevx = 0;
    float prevy = 0;
};

#endif // VKMAINWINDOW_H
