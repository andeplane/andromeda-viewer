#pragma once
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <vector>

struct PointsData {
    std::vector<QVector2D> positions;
    std::vector<QVector2D> velocities;
};

class Points
{
public:
    Points();
    ~Points();
    void update(const PointsData &data);
    void render();
private:
    GLuint m_vboIds[1];
    unsigned int m_numberOfPoints;
    QOpenGLFunctions *m_funcs;
    QOpenGLShaderProgram *m_program;

    void createShaderProgram();
    void generateVBOs();
    void ensureInitialized();
};
