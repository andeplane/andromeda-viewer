#include "renderer.h"
#include "controller.h"
#include "points.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QGLFormat>
#include <QOpenGLContext>
#include <iostream>
#include <cmath>
#include <QFileInfo>
#include <QApplication>
#include <QOpenGLFramebufferObjectFormat>

using namespace std;
namespace CompPhys {

void Renderer::synchronize(QQuickFramebufferObject* item)
{
    m_syncCount++;
    Controller *controller = (Controller*)item; // TODO: Use correct casting method
    if(!controller) {
        return;
    }

    m_renderAndromeda1x = controller->renderAndromeda1x();
    m_renderAndromeda2x = controller->renderAndromeda2x();
    m_renderAndromeda3x = controller->renderAndromeda3x();
    m_renderSky = controller->renderSky();

    resetProjection();
    setModelViewMatrix(controller->cameraPos().x(), controller->cameraPos().y(), -controller->cameraPos().z());
    if(controller->simulatorOutputDirty()) {
        controller->m_simulatorOutputMutex.lock();
        // Read output

        m_andromeda1x->update(m_andromeda1xData.billboardsData);
        m_andromeda2x->update(m_andromeda2xData.billboardsData);
        m_andromeda3x->update(m_andromeda3xData.billboardsData);
        m_sky->update(m_skyData.billboardsData);

        controller->setSimulatorOutputDirty(false);
        controller->m_simulatorOutputMutex.unlock();
        m_dirtyCount++;
    }
}

void Renderer::render()
{
    m_renderCount++;
    glDepthMask(true);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    // Render data
    QMatrix4x4 modelViewProjectionMatrix = m_projectionMatrix*m_modelViewMatrix;
    if(m_renderSky) m_sky->render(modelViewProjectionMatrix);
    if(m_renderAndromeda1x) m_andromeda1x->render(modelViewProjectionMatrix);
    if(m_renderAndromeda2x) m_andromeda2x->render(modelViewProjectionMatrix);
    if(m_renderAndromeda3x) m_andromeda3x->render(modelViewProjectionMatrix);
}
QSize Renderer::viewportSize() const
{
    return m_viewportSize;
}

void Renderer::setViewportSize(const QSize &viewportSize)
{
    m_viewportSize = viewportSize;
}



Renderer::Renderer() :
    m_skipNextFrame(false),
    m_syncCount(0),
    m_renderCount(0),
    m_dirtyCount(0),
    m_renderAndromeda1x(true),
    m_renderAndromeda2x(true),
    m_renderAndromeda3x(true)
{
    bool lowResolution = true;

    QString executablePath = QFileInfo( QCoreApplication::applicationFilePath() ).path();
    QString imagePath = executablePath.append("/../andromeda-viewer/");

    QString path1x = imagePath + "andromeda_1x.jpg";
    QString path2x = imagePath + "andromeda_2x.jpg";
    QString path3x = imagePath + "andromeda_3x.jpg";

    if(lowResolution) {
        path2x = imagePath + "andromeda_2x_low.jpg";
        path3x = imagePath + "andromeda_3x_low.jpg";
    }

    m_andromeda1x = new Billboards(path1x);
    m_andromeda2x = new Billboards(path2x);
    m_andromeda3x = new Billboards(path3x);


    QString skyPath = imagePath + "andromeda_sky.jpg";
    m_sky = new Billboards(skyPath);
    m_skyData.billboardsData.positions.push_back(QVector2D(0,0));
    m_skyData.billboardsData.scale = 1.0;
    m_skyData.billboardsData.aspectRatio = 1.33351618;

    m_andromeda1xData.billboardsData.positions.push_back(QVector2D(0.41975,-0.4975));
    m_andromeda1xData.billboardsData.scale = 0.06775;
    m_andromeda1xData.billboardsData.rotation.push_back(-0.25*M_PI + M_PI);
    m_andromeda1xData.billboardsData.aspectRatio = 1.52207002;

    m_andromeda2xData.billboardsData.positions.push_back(QVector2D(0.404525,-0.4921));
    m_andromeda2xData.billboardsData.scale = 0.01465;
    m_andromeda2xData.billboardsData.rotation.push_back(M_PI*(1 - 0.0445));
    m_andromeda2xData.billboardsData.aspectRatio = 3.1282586;

    float deltaX =  0.0000075;
    float deltaY = -0.000016;
    m_andromeda3xData.billboardsData.positions.push_back(QVector2D(0.39361 + deltaX,-0.48935 - deltaY));
    m_andromeda3xData.billboardsData.scale = 0.003463;
    m_andromeda3xData.billboardsData.rotation.push_back(M_PI*(1 - 0.0445));
    m_andromeda3xData.billboardsData.aspectRatio = 1.0;
}

Renderer::~Renderer()
{

}

void Renderer::resetProjection()
{
    m_viewportSize = QSize(1080, 1080);
    // Calculate aspect ratio
    qreal aspect = qreal(m_viewportSize.width()) / qreal(m_viewportSize.height() ? m_viewportSize.height() : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 65 degrees
    const qreal zNear = 0.00001, zFar = 2000.0, fov = 90.0;

    // Reset projection
    m_projectionMatrix.setToIdentity();

    // Set perspective projection
    m_projectionMatrix.perspective(fov, aspect, zNear, zFar);
}

QOpenGLFramebufferObject *Renderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void Renderer::setModelViewMatrix(double x, double y, double z)
{
    m_modelViewMatrix.setToIdentity();
    m_modelViewMatrix.translate(x,y,z);
}
}
