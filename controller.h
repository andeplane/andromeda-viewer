/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QQuickFramebufferObject>
#include <QThread>
#include <QMutex>
#include "simulator.h"
#include "renderer.h"
namespace CompPhys {
class Controller : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool previousStepCompleted READ previousStepCompleted NOTIFY previousStepCompletedChanged)
    Q_PROPERTY(QVector3D cameraPos READ cameraPos WRITE setCameraPos NOTIFY cameraPosChanged)
    Q_PROPERTY(bool renderAndromeda1x READ renderAndromeda1x WRITE setRenderAndromeda1x NOTIFY renderAndromeda1xChanged)
    Q_PROPERTY(bool renderAndromeda2x READ renderAndromeda2x WRITE setRenderAndromeda2x NOTIFY renderAndromeda2xChanged)
    Q_PROPERTY(bool renderAndromeda3x READ renderAndromeda3x WRITE setRenderAndromeda3x NOTIFY renderAndromeda3xChanged)
    Q_PROPERTY(bool renderSky READ renderSky WRITE setRenderSky NOTIFY renderSkyChanged)
public:
    Controller();
    ~Controller();

    CompPhys::Renderer* createRenderer() const;

    Q_INVOKABLE void step();

    bool running() const;
    bool previousStepCompleted() const;
    bool simulatorOutputDirty() const;

    QVector3D cameraPos() const
    {
        return m_cameraPos;
    }

    bool renderAndromeda1x() const
    {
        return m_renderAndromeda1x;
    }

    bool renderAndromeda2x() const
    {
        return m_renderAndromeda2x;
    }

    bool renderAndromeda3x() const
    {
        return m_renderAndromeda3x;
    }

    bool renderSky() const
    {
        return m_renderSky;
    }



public slots:
    void setRunning(bool arg);
    void setPreviousStepCompleted(bool arg);
    void setSimulatorOutputDirty(bool arg);

    void setCameraPos(QVector3D arg)
    {
        if (m_cameraPos == arg)
            return;

        m_cameraPos = arg;
        emit cameraPosChanged(arg);
    }

    void setRenderSky(bool arg)
    {
        if (m_renderSky == arg)
            return;

        m_renderSky = arg;
        emit renderSkyChanged(arg);
    }

    void setRenderAndromeda3x(bool arg)
    {
        if (m_renderAndromeda3x == arg)
            return;

        m_renderAndromeda3x = arg;
        emit renderAndromeda3xChanged(arg);
    }

    void setRenderAndromeda2x(bool arg)
    {
        if (m_renderAndromeda2x == arg)
            return;

        m_renderAndromeda2x = arg;
        emit renderAndromeda2xChanged(arg);
    }

    void setRenderAndromeda1x(bool arg)
    {
        if (m_renderAndromeda1x == arg)
            return;

        m_renderAndromeda1x = arg;
        emit renderAndromeda1xChanged(arg);
    }

private slots:
    void finalizeStep();

signals:
    void runningChanged(bool arg);
    void previousStepCompletedChanged(bool arg);
    void requestStep();

    void cameraPosChanged(QVector3D arg);

    void renderSkyChanged(bool arg);

    void renderAndromeda3xChanged(bool arg);

    void renderAndromeda2xChanged(bool arg);

    void renderAndromeda1xChanged(bool arg);

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    CompPhys::Renderer *m_renderer;
    CompPhys::VisualData m_visualData;
    Simulator m_simulator;
    QElapsedTimer m_timer;
    bool m_running;

    bool m_stepRequested;
    bool m_previousStepCompleted;

    bool m_simulatorOutputDirty;
    bool m_lastStepWasBlocked;

    QMutex m_simulatorInputMutex;
    QMutex m_simulatorOutputMutex;
    QMutex m_simulatorRunningMutex;
    QThread m_simulatorWorker;
    friend class CompPhys::Renderer;
    QVector3D m_cameraPos;
    bool m_renderSky;
    bool m_renderAndromeda3x;
    bool m_renderAndromeda2x;
    bool m_renderAndromeda1x;
};
}
