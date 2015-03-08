#pragma once

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QQuickFramebufferObject>
#include <QThread>
#include <QMutex>
#include "points.h"
#include "billboards.h"

namespace CompPhys {
struct VisualData {
    BillboardsData billboardsData;
};

class Renderer : public QQuickFramebufferObject::Renderer
{
public:
    Renderer();
    ~Renderer();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void setModelViewMatrix(double x, double y, double z);
    void synchronize(QQuickFramebufferObject *item);
    void render();

    QSize viewportSize() const;
    void setViewportSize(const QSize &viewportSize);

    void resetProjection();
private:
    QMatrix4x4 m_projectionMatrix;
    QSize m_viewportSize;

    bool m_skipNextFrame;
    Billboards *m_sky;
    Billboards *m_andromeda1x;
    Billboards *m_andromeda2x;
    Billboards *m_andromeda3x;
    VisualData m_skyData;
    VisualData m_andromeda1xData;
    VisualData m_andromeda2xData;
    VisualData m_andromeda3xData;
    QMatrix4x4 m_modelViewMatrix;
    bool m_renderAndromeda1x;
    bool m_renderAndromeda2x;
    bool m_renderAndromeda3x;
    bool m_renderSky;

    int m_syncCount;
    int m_renderCount;
    int m_dirtyCount;
};
}
