#ifndef PICTUREVIEW_H
#define PICTUREVIEW_H

#include <QQuickItem>
#include <QQmlEngine>
#include <QOpenGLFunctions>
#include <QQuickWindow>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class VideoGLRenderer;

class GLVideoWidget : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    GLVideoWidget(QQuickItem *parent = nullptr);
    ~GLVideoWidget();

public slots:
    void handleWindowChanged(QQuickWindow *window);
    void guiSync();
    void cleanup();

protected:
    void releaseResources() override;

private:
    VideoGLRenderer *vgl_renderer;



};

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;
class QOpenGLTexture;

class VideoGLRenderer : public QObject,
                        protected QOpenGLFunctions

{
    Q_OBJECT

public:
    explicit VideoGLRenderer(QObject *parent = nullptr);
    ~VideoGLRenderer();

public slots:
    void initRenderer();
    void paint();
    void setWindow(QQuickWindow *window);


protected:
private:
    QQuickWindow *quick_window;
    QOpenGLShaderProgram *shader_program;
    QOpenGLBuffer *vbo;
    QOpenGLBuffer *ebo;
    QOpenGLVertexArrayObject *vao;
    QOpenGLTexture *video_frame_texture;

    QMutex mutex;


};

#endif // PICTUREVIEW_H
