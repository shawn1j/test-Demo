#include "GLVideoWidget.h"

#include <QOpenGLShaderProgram>
#include <QSGRendererInterface>
#include <QQuickWindow>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QImage>
#include <QRunnable>


GLVideoWidget::GLVideoWidget(QQuickItem *parent)
    : QQuickItem(parent)
    , vgl_renderer(nullptr)


{
    setFlag(ItemHasContents, true);

    connect(this, &QQuickItem::windowChanged, this, &GLVideoWidget::handleWindowChanged);

}

GLVideoWidget::~GLVideoWidget()
{

}

void GLVideoWidget::handleWindowChanged(QQuickWindow *window)
{
    if (window) {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &GLVideoWidget::guiSync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &GLVideoWidget::cleanup, Qt::DirectConnection);
    }
}

void GLVideoWidget::guiSync()
{
    if (this->vgl_renderer == nullptr) {
        this->vgl_renderer = new VideoGLRenderer(this);

        connect(window(), &QQuickWindow::beforeRendering, this->vgl_renderer, &VideoGLRenderer::initRenderer,
                Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording, this->vgl_renderer, &VideoGLRenderer::paint,
                Qt::DirectConnection);



    }
    this->vgl_renderer->setWindow(window());
}

void GLVideoWidget::cleanup()
{
    delete this->vgl_renderer;
    this->vgl_renderer = nullptr;
}


class VideoRenderer;

class CleanupJob : public QRunnable
{
public:
    CleanupJob(VideoGLRenderer *renderer) : renderer(renderer) {}

    void run() override
    {
        if (this->renderer) {

            delete this->renderer;
        }
    }

private:
    VideoGLRenderer *renderer;
};

void GLVideoWidget::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(this->vgl_renderer), QQuickWindow::BeforeSynchronizingStage);
    this->vgl_renderer = nullptr;
}



VideoGLRenderer::VideoGLRenderer(QObject *parent) : quick_window(nullptr), shader_program(nullptr){}

VideoGLRenderer::~VideoGLRenderer() { }



void VideoGLRenderer::initRenderer()
{
    QString vertexSource = R"(
        #version 330 core
        layout(location = 0) in vec3 vPosition;
        layout(location = 1) in vec2 vTexCoord;
        out vec2 TexCoord;
        uniform mat4 zoomMatrix; 
        void main() { 
            gl_Position = zoomMatrix * vec4(vPosition, 1.0); 
            TexCoord = vTexCoord; 
        }
        )";

    QString fragSource = R"(
        #version 330 core 
        in vec2 TexCoord; 
        out vec4 FragColor; 
        uniform sampler2D frameTexture; 
        uniform bool IsTextureValid; 
        void main() { 
            if(!IsTextureValid) 
            { 
                FragColor = vec4(0.7,0.2,0.5,1.0);
            } else { 
                FragColor = texture(frameTexture, TexCoord); 
            } 
        }
        )";

    if (this->shader_program != nullptr)
        return;

    QSGRendererInterface *rif = this->quick_window->rendererInterface();
    Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

    initializeOpenGLFunctions();

    this->shader_program = new QOpenGLShaderProgram();
    if (!this->shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource))
        qDebug() << this->shader_program->log();
    if (!this->shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource))
        qDebug() << this->shader_program->log();
    if (!this->shader_program->link())
        qDebug() << this->shader_program->log();

    QVector<GLfloat> vertices = {
        // positions         // texture coords
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
    };

    QVector<GLuint> indices = {
        0, 1, 2, 2, 3, 0,
    };

    this->vao = new QOpenGLVertexArrayObject();
    this->vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    this->ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    this->vao->create();
    this->vbo->create();
    this->ebo->create();

    this->vao->bind();
    this->vbo->bind();
    this->ebo->bind();

    this->vbo->allocate(vertices.constData(), vertices.size() * sizeof(GLfloat));
    this->ebo->allocate(indices.constData(), indices.size() * sizeof(GLuint));

    this->shader_program->bind();

    this->shader_program->setAttributeBuffer("vPosition", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    this->shader_program->enableAttributeArray(0);
    this->shader_program->setAttributeBuffer("vTexCoord", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    this->shader_program->enableAttributeArray(1);

    this->vao->release();

    this->video_frame_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    this->video_frame_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    this->video_frame_texture->setMinificationFilter(QOpenGLTexture::Linear);
    this->video_frame_texture->setWrapMode(QOpenGLTexture::Repeat);
}

void VideoGLRenderer::paint()
{
    bool isTextureValid;
    QMatrix4x4 zoom;

    this->quick_window->beginExternalCommands();

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);



    QImage img("E:/qt_project/qmlTestProject/test/fufu.png");
    if (img.isNull()) {

        return;
    }

    video_frame_texture->destroy();
    video_frame_texture->create();

    video_frame_texture->setData(img.mirrored());

    isTextureValid = false;

    this->shader_program->bind();
    this->shader_program->setUniformValue("IsTextureValid", isTextureValid);

    this->vao->bind();
    if (isTextureValid) {

        this->video_frame_texture->bind(0);
    }
    this->shader_program->setUniformValue("zoomMatrix", zoom);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    this->vao->release();
    this->shader_program->release();
    if (isTextureValid)
        this->video_frame_texture->release();

    this->quick_window->endExternalCommands();
}

void VideoGLRenderer::setWindow(QQuickWindow *window) { this->quick_window = window; }


