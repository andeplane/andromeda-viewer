#include <billboards.h>
#include <QOpenGLFunctions>
#include <QColor>
#include <QRgb>
#include <cmath>

using std::vector;

Billboards::Billboards(QString texturePath) :
    m_funcs(0),
    m_program(0),
    m_opacity(1.0)
{
    generateVBOs();
    uploadTexture(texturePath);
}

Billboards::~Billboards()
{
    if(m_funcs) delete m_funcs;
    if(m_program) delete m_program;
}

void Billboards::generateVBOs()
{
    ensureInitialized();
    m_funcs->glGenBuffers(2, m_vboIds);
}

void Billboards::uploadTexture(QString texturePath)
{
    m_texture = new QOpenGLTexture(QImage(QString(texturePath)).mirrored());
}

void Billboards::ensureInitialized()
{
    if(!m_funcs) m_funcs = new QOpenGLFunctions(QOpenGLContext::currentContext());
}

QVector3D Billboards::vectorFromColor(const QColor &color)
{
    return QVector3D(color.redF(), color.greenF(), color.blueF());
}

// void Billboards::update(vector<float> &positions)
void Billboards::update(BillboardsData &data)
{
    ensureInitialized();
    vector<QVector2D> &positions = data.positions;
    vector<float> &rotation = data.rotation;

    QVector2D right;
    right.setX(1.0);
    right.setY(0.0);
    QVector2D up;
    up.setX(0.0);
    up.setY(1.0);

    QVector2D ul = (up/data.aspectRatio - right)*data.scale;
    QVector2D ur = (up/data.aspectRatio + right)*data.scale;
    QVector2D dl = (-up/data.aspectRatio - right)*data.scale;
    QVector2D dr = (-up/data.aspectRatio + right)*data.scale;

    int numberOfVertices = data.positions.size()*4;
    m_vertices.resize(numberOfVertices);
    m_indices.resize(6*data.positions.size());

    QVector3D normalColor = vectorFromColor(QColor("#1f78b4"));

    for(int i=0; i<data.positions.size(); i++) {
        // NOTE: Y and Z are swapped!
        QVector2D &position = positions[i];
        float rot = 0;
        if(rotation.size() > 1) {
            rot = rotation[i];
        } else if(rotation.size() == 1) {
            rot = rotation[0];
        }
        float cosTheta = cos(rot);
        float sinTheta = sin(rot);

        //m_vertices[4*i + 0].position = position + dl;
        m_vertices[4*i + 0].position = position;
        m_vertices[4*i + 0].position[0] += dl[0]*cosTheta - dl[1]*sinTheta;
        m_vertices[4*i + 0].position[1] += dl[0]*sinTheta + dl[1]*cosTheta;
        m_vertices[4*i + 0].textureCoord= QVector2D(0,1);

        // m_vertices[4*i + 0].position = position + dr;
        m_vertices[4*i + 1].position = position;
        m_vertices[4*i + 1].position[0] += dr[0]*cosTheta - dr[1]*sinTheta;
        m_vertices[4*i + 1].position[1] += dr[0]*sinTheta + dr[1]*cosTheta;
        m_vertices[4*i + 1].textureCoord= QVector2D(1,1);

        // m_vertices[4*i + 2].position = position + ur;
        m_vertices[4*i + 2].position = position;
        m_vertices[4*i + 2].position[0] += ur[0]*cosTheta - ur[1]*sinTheta;
        m_vertices[4*i + 2].position[1] += ur[0]*sinTheta + ur[1]*cosTheta;
        m_vertices[4*i + 2].textureCoord= QVector2D(1,0);

        // m_vertices[4*i + 3].position = position + ul;
        m_vertices[4*i + 3].position = position;
        m_vertices[4*i + 3].position[0] += ul[0]*cosTheta - ul[1]*sinTheta;
        m_vertices[4*i + 3].position[1] += ul[0]*sinTheta + ul[1]*cosTheta;
        m_vertices[4*i + 3].textureCoord= QVector2D(0,0);

        QVector3D color = normalColor;

        m_vertices[4*i + 0].color = color;
        m_vertices[4*i + 1].color = color;
        m_vertices[4*i + 2].color = color;
        m_vertices[4*i + 3].color = color;

        m_indices [6*i + 0] = 4*i+0;
        m_indices [6*i + 1] = 4*i+1;
        m_indices [6*i + 2] = 4*i+2;

        m_indices [6*i + 3] = 4*i+2;
        m_indices [6*i + 4] = 4*i+3;
        m_indices [6*i + 5] = 4*i+0;
    }

    // Transfer vertex data to VBO 0
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexData), &m_vertices[0], GL_STATIC_DRAW);

    // Transfer index data to VBO 1
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    m_funcs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);
}

void Billboards::createShaderProgram() {
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();

        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec2 a_position;\n"
                                           "attribute highp vec3 a_color;\n"
                                           "attribute highp vec2 a_texcoord;\n"
                                           "uniform highp mat4 modelViewProjectionMatrix;\n"
                                           "varying highp vec2 coords;\n"
                                           "varying highp float light;\n"
                                           "varying highp vec3 color;\n"
                                           "void main() {\n"
                                           "    gl_Position = modelViewProjectionMatrix*vec4(a_position, 0.0, 1.0);\n"
                                           "    coords = a_texcoord;\n"
                                           "    color = a_color;\n"
                                           "}");

        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "uniform sampler2D texture;\n"
                                           "uniform float opacity;\n"
                                           "varying highp vec2 coords;\n"
                                           "varying highp vec3 color;\n"
                                           "void main() {\n"
                                           "    gl_FragColor = texture2D(texture, coords.st);\n"
                                           "    gl_FragColor.a = opacity;"
                                           "}");


        m_program->link();
    }
}

void Billboards::render(QMatrix4x4 &modelViewProjectionMatrix)
{
    if(m_vertices.size() == 0) return;
    ensureInitialized();
    createShaderProgram();

    m_program->bind();

    m_program->setUniformValue("modelViewProjectionMatrix", modelViewProjectionMatrix);
    m_program->setUniformValue("opacity", m_opacity);

    // Tell OpenGL which VBOs to use
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = m_program->attributeLocation("a_position");
    m_program->enableAttributeArray(vertexLocation);
    m_funcs->glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector2D);

    // Tell OpenGL programmable pipeline how to locate vertex color data
    int colorLocation = m_program->attributeLocation("a_color");
    m_program->enableAttributeArray(colorLocation);
    m_funcs->glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = m_program->attributeLocation("a_texcoord");
    m_program->enableAttributeArray(texcoordLocation);
    m_funcs->glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

    // Draw cube geometry using indices from VBO 1
    // glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
    m_texture->bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    m_program->disableAttributeArray(vertexLocation);
    m_program->disableAttributeArray(colorLocation);
    m_program->disableAttributeArray(texcoordLocation);

    m_program->release();
}
float Billboards::opacity() const
{
    return m_opacity;
}

void Billboards::setOpacity(float opacity)
{
    m_opacity = opacity;
}

