#pragma once
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <vector>
using std::vector;

struct BillboardsData
{
    vector<QVector2D> positions;
    vector<float> rotation;
    float scale = 1.0;
    float aspectRatio = 1.0;
};

struct VertexData
{
    QVector2D position;
    QVector3D color;
    QVector2D textureCoord;
};

class Billboards
{
public:
    Billboards(QString texturePath);
    ~Billboards();
    void update(BillboardsData &data);
    void render(QMatrix4x4 &modelViewProjectionMatrix);
    float opacity() const;
    void setOpacity(float opacity);

private:
    GLuint m_vboIds[2];
    std::vector<VertexData> m_vertices;
    std::vector<GLuint> m_indices;
    QOpenGLFunctions *m_funcs = 0;
    QOpenGLShaderProgram *m_program = 0;
    QOpenGLTexture *m_texture = 0;
    QMatrix4x4 m_modelViewMatrix;
    float m_opacity;

    void createShaderProgram();
    void generateVBOs();
    void uploadTexture(QString texturePath);
    void ensureInitialized();
    QVector3D vectorFromColor(const QColor &color);
};
