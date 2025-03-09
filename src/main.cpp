#include "Object.h"
#include "World.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>
#include <FL/platform.H>
#include <stdarg.h>
#include <stdio.h>
#if defined(__APPLE__)
#    include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
// Note: GLEW_STATIC is defined by CMake if the static lib is linked
#    include <GL/glew.h>
#endif
#ifdef _WIN32
#include <GL/wglew.h>
#endif

#include <FL/gl.h> // for gl_texture_reset()

namespace
{
class GLWindow: public Fl_Gl_Window
{
public:
    GLWindow(int32_t w, int32_t h, const char* name);

private:
    void draw() override;
    int32_t handle(int32_t event) override;
    void create_program();

    void add_output(const char* format, ...);

    int32_t first_;
    GLuint shaderProgram_;
    GLuint vertexArrayObject_;
    GLuint vertexBuffer_;
    GLint matrixUniform_;
    GLint colourAttribute_;
    GLint positionAttribute_;
};

GLWindow::GLWindow(int32_t w, int32_t h, const char* name)
    : Fl_Gl_Window(w, h, name)
    , first_(1)
    , shaderProgram_(0)
    , vertexArrayObject_(0)
    , vertexBuffer_(0)
    , matrixUniform_(0)
    , colourAttribute_(0)
    , positionAttribute_(0)
{
    mode(FL_RGB8 | FL_DOUBLE | FL_DEPTH | FL_OPENGL3);
}

void GLWindow::draw()
{
    //Fl_Gl_Window::draw();
    if(!valid()) {
        glViewport(0, 0, pixel_w(), pixel_h());
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray((GLuint)positionAttribute_);
    glEnableVertexAttribArray((GLuint)colourAttribute_);
    glVertexAttribPointer((GLuint)positionAttribute_, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glVertexAttribPointer((GLuint)colourAttribute_, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (char*)0 + 4 * sizeof(GLfloat));
    glUseProgram(shaderProgram_);

    GLfloat m[16] = {};
    glUniformMatrix4fv(matrixUniform_, 1, GL_FALSE, (const GLfloat*)m);
    glUniform2fv(matrixUniform_, 16, (const GLfloat*)&m);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    swap_buffers();
}

int32_t GLWindow::handle(int32_t event)
{
    switch(event) {
    case FL_PUSH:
        return 1;
    case FL_DRAG:
        return 1;
    case FL_RELEASE:
        return 1;
    case FL_FOCUS:
    case FL_UNFOCUS:
        return 1;
    case FL_KEYBOARD:
        return 1;
    case FL_SHORTCUT:
        return 1;
    case FL_SHOW:
        if(first_ && shown()) {
            first_ = 0;
            make_current();
            GLenum err = glewInit();
            if(err) {
                Fl::warning("glewInit() failed returning %u", err);
            } else {
                add_output("using GLEW %s\n", glewGetString(GLEW_VERSION));
            }
            //wglSwapIntervalEXT(1);
            if(!shaderProgram_){
                create_program();
            }
            redraw();
        }
        return 1;
    default:
        return Fl_Gl_Window::handle(event);
    }
}

void GLWindow::create_program()
{
    GLuint vs;
    GLuint fs;
    int Mslv, mslv; // major and minor version numbers of the shading language
    sscanf((char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%d.%d", &Mslv, &mslv);
    add_output("Shading Language Version=%d.%d\n", Mslv, mslv);
    const char* vss_format =
        "#version %d%d\n"
        "uniform mat44 m;\n"
        "in vec4 position;\n"
        "in vec4 colour;\n"
        "out vec4 colourV;\n"
        "void main (void)\n"
        "{\n"
        "   colourV = colour;\n"
        "   gl_Position = mul(m, position);\n"
        "}";
    char vss_string[300];
    const char* vss = vss_string;
    snprintf(vss_string, 300, vss_format, Mslv, mslv);
    const char* fss_format =
        "#version %d%d\n"
        "in vec4 colourV;\n"
        "out vec4 fragColour;\n"
        "void main(void)\n"
        "{\n"
        "   fragColour = colourV;\n"
        "}";
    char fss_string[200];
    const char* fss = fss_string;
    snprintf(fss_string, 200, fss_format, Mslv, mslv);
    GLint err;
    GLchar CLOG[1000];
    GLsizei length;
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vss, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &err);
    if(err != GL_TRUE) {
        glGetShaderInfoLog(vs, sizeof(CLOG), &length, CLOG);
        add_output("vs ShaderInfoLog=%s\n", CLOG);
    }
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fss, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &err);
    if(err != GL_TRUE) {
        glGetShaderInfoLog(fs, sizeof(CLOG), &length, CLOG);
        add_output("fs ShaderInfoLog=%s\n", CLOG);
    }
    // Attach the shaders
    shaderProgram_ = glCreateProgram();
    glAttachShader(shaderProgram_, vs);
    glAttachShader(shaderProgram_, fs);
    glBindFragDataLocation(shaderProgram_, 0, "fragColour");
    glLinkProgram(shaderProgram_);
    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &err);
    if(err != GL_TRUE) {
        glGetProgramInfoLog(shaderProgram_, sizeof(CLOG), &length, CLOG);
        add_output("link log=%s\n", CLOG);
    }
    // Get pointers to uniforms and attributes
    matrixUniform_ = glGetUniformLocation(shaderProgram_, "m");
    colourAttribute_ = glGetAttribLocation(shaderProgram_, "colour");
    positionAttribute_ = glGetAttribLocation(shaderProgram_, "position");
    glDeleteShader(vs);
    glDeleteShader(fs);
    // Upload vertices (1st four values in a row) and colours (following four values)
    GLfloat vertexData[] = {-0.5, -0.5, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
                            -0.5, 0.5, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
                            0.5, 0.5, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,
                            0.5, -0.5, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    glGenVertexArrays(1, &vertexArrayObject_);
    glBindVertexArray(vertexArrayObject_);

    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, 4 * 8 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    //glEnableVertexAttribArray((GLuint)positionAttribute_);
    //glEnableVertexAttribArray((GLuint)colourAttribute_);
    //glVertexAttribPointer((GLuint)positionAttribute_, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    //glVertexAttribPointer((GLuint)colourAttribute_, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (char*)0 + 4 * sizeof(GLfloat));
    //glUseProgram(shaderProgram_);
}

void GLWindow::add_output(const char* format, ...)
{
    // va_list args;
    // char line_buffer[512];
    // va_start(args, format);
    // vsnprintf(line_buffer, sizeof(line_buffer) - 1, format, args);
    // va_end(args);
    // text_display_->buffer()->append(line_buffer);
    // text_display_->scroll(512, 0);
    // text_display_->redraw();
}

} // namespace

int main(int argc, char** argv)
{
    using namespace lray;
    // Fl_Window* top = new Fl_Window(640, 600);
    GLWindow* win = new GLWindow(640, 480, "GL");
    win->end();
    win->show(argc, argv);
    return Fl::run();
}
