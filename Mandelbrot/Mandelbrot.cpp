#include <GL/freeglut.h>

#include <cstdio>
#include <iostream>

// Шейдерные константы OpenGL 2.0 (Windows gl.h знает только GL 1.1)
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER          0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER        0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS         0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS            0x8B82
#endif
#ifndef GL_SHADING_LANGUAGE_VERSION
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif

// Параметры отображения
const int WIDTH = 800;
const int HEIGHT = 800;
const int MAX_ITERATIONS = 1000;

// Параметры фрактала
double centerX = -0.5;
double centerY = 0.0;
double scale = 2.0;

// --- Мини-лоадер OpenGL 2.0 функций через freeglut (GLEW не нужен) ---
typedef GLuint(*PFN_CREATESHADER)(GLenum);
typedef void (*PFN_SHADERSOURCE)(GLuint, GLsizei, const char* const*, const GLint*);
typedef void (*PFN_COMPILESHADER)(GLuint);
typedef void (*PFN_GETSHADERIV)(GLuint, GLenum, GLint*);
typedef void (*PFN_GETSHADERINFOLOG)(GLuint, GLsizei, GLsizei*, char*);
typedef GLuint(*PFN_CREATEPROGRAM)(void);
typedef void (*PFN_ATTACHSHADER)(GLuint, GLuint);
typedef void (*PFN_LINKPROGRAM)(GLuint);
typedef void (*PFN_GETPROGRAMIV)(GLuint, GLenum, GLint*);
typedef void (*PFN_GETPROGRAMINFOLOG)(GLuint, GLsizei, GLsizei*, char*);
typedef void (*PFN_USEPROGRAM)(GLuint);
typedef GLint(*PFN_GETUNIFORMLOCATION)(GLuint, const char*);
typedef void (*PFN_UNIFORM1I)(GLint, GLint);
typedef void (*PFN_UNIFORM1F)(GLint, GLfloat);
typedef void (*PFN_UNIFORM2F)(GLint, GLfloat, GLfloat);

static PFN_CREATESHADER p_glCreateShader;
static PFN_SHADERSOURCE p_glShaderSource;
static PFN_COMPILESHADER p_glCompileShader;
static PFN_GETSHADERIV p_glGetShaderiv;
static PFN_GETSHADERINFOLOG p_glGetShaderInfoLog;
static PFN_CREATEPROGRAM p_glCreateProgram;
static PFN_ATTACHSHADER p_glAttachShader;
static PFN_LINKPROGRAM p_glLinkProgram;
static PFN_GETPROGRAMIV p_glGetProgramiv;
static PFN_GETPROGRAMINFOLOG p_glGetProgramInfoLog;
static PFN_USEPROGRAM p_glUseProgram;
static PFN_GETUNIFORMLOCATION p_glGetUniformLocation;
static PFN_UNIFORM1I p_glUniform1i;
static PFN_UNIFORM1F p_glUniform1f;
static PFN_UNIFORM2F p_glUniform2f;

#define LOADGL(ptr, name) \
    do { \
        ptr = (decltype(ptr))glutGetProcAddress(name); \
        if (!ptr) { std::cerr << "Не найдена GL-функция: " << name << std::endl; return 0; } \
    } while (0)

// Состояние шейдера
static GLuint program = 0;
static GLint locCenter = -1, locResolution = -1, locScale = -1, locMaxIter = -1;

// gl_FragCoord даёт координаты в пикселях; float-мантисса ограничивает
// глубину зума примерно до 1e5 (дальше пикселизались бы). Для более
// глубокого зума нужна double-эмуляция (double-single) или perturbation.
static const char* VERT_SRC =
    "#version 120\n"
    "void main() { gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0); }\n";

static const char* FRAG_SRC =
    "#version 120\n"
    "uniform vec2  uCenter;\n"
    "uniform vec2  uResolution;\n"
    "uniform float uScale;\n"
    "uniform int   uMaxIter;\n"
    "void main() {\n"
    "    float sf = uScale / uResolution.x;\n"
    "    vec2  c  = uCenter + (gl_FragCoord.xy - uResolution * 0.5) * sf;\n"
    "    float x = 0.0, y = 0.0, x2 = 0.0, y2 = 0.0;\n"
    "    int iter = 0;\n"
    "    for (; iter < uMaxIter; ++iter) {\n"
    "        y  = 2.0 * x * y + c.y;\n"
    "        x  = x2 - y2 + c.x;\n"
    "        x2 = x * x;\n"
    "        y2 = y * y;\n"
    "        if (x2 + y2 > 4.0) break;\n"
    "    }\n"
    "    vec3 rgb;\n"
    "    if (iter == uMaxIter) {\n"
    "        rgb = vec3(0.0);\n"
    "    } else {\n"
    "        float t = float(iter) / float(uMaxIter);\n"
    "        rgb = vec3(t * 4.0, t * 2.0, t);\n"
    "        if (rgb.r > 1.0) rgb.r = 2.0 - rgb.r;\n"
    "        if (rgb.g > 1.0) rgb.g = 2.0 - rgb.g;\n"
    "        if (rgb.b > 1.0) rgb.b = 2.0 - rgb.b;\n"
    "    }\n"
    "    gl_FragColor = vec4(rgb, 1.0);\n"
    "}\n";

GLuint compileShader(GLenum type, const char* src) {
    GLuint s = p_glCreateShader(type);
    p_glShaderSource(s, 1, &src, nullptr);
    p_glCompileShader(s);

    GLint ok = 0;
    p_glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        p_glGetShaderInfoLog(s, (GLsizei)sizeof(log), nullptr, log);
        std::cerr << "Ошибка компиляции шейдера:\n" << log << std::endl;
        exit(1);
    }
    return s;
}

int initGL() {
    LOADGL(p_glCreateShader, "glCreateShader");
    LOADGL(p_glShaderSource, "glShaderSource");
    LOADGL(p_glCompileShader, "glCompileShader");
    LOADGL(p_glGetShaderiv, "glGetShaderiv");
    LOADGL(p_glGetShaderInfoLog, "glGetShaderInfoLog");
    LOADGL(p_glCreateProgram, "glCreateProgram");
    LOADGL(p_glAttachShader, "glAttachShader");
    LOADGL(p_glLinkProgram, "glLinkProgram");
    LOADGL(p_glGetProgramiv, "glGetProgramiv");
    LOADGL(p_glGetProgramInfoLog, "glGetProgramInfoLog");
    LOADGL(p_glUseProgram, "glUseProgram");
    LOADGL(p_glGetUniformLocation, "glGetUniformLocation");
    LOADGL(p_glUniform1i, "glUniform1i");
    LOADGL(p_glUniform1f, "glUniform1f");
    LOADGL(p_glUniform2f, "glUniform2f");

    GLuint vs = compileShader(GL_VERTEX_SHADER, VERT_SRC);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, FRAG_SRC);
    program = p_glCreateProgram();
    p_glAttachShader(program, vs);
    p_glAttachShader(program, fs);
    p_glLinkProgram(program);

    GLint ok = 0;
    p_glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        p_glGetProgramInfoLog(program, (GLsizei)sizeof(log), nullptr, log);
        std::cerr << "Ошибка линковки программы:\n" << log << std::endl;
        return 0;
    }

    p_glUseProgram(program);
    locCenter = p_glGetUniformLocation(program, "uCenter");
    locResolution = p_glGetUniformLocation(program, "uResolution");
    locScale = p_glGetUniformLocation(program, "uScale");
    locMaxIter = p_glGetUniformLocation(program, "uMaxIter");
    return 1;
}

// Отрисовка: один fullscreen-квад, фрактал считается во фрагментном шейдере
void display() {
    int t0 = glutGet(GLUT_ELAPSED_TIME);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT);

    p_glUseProgram(program);
    p_glUniform2f(locCenter, (GLfloat)centerX, (GLfloat)centerY);
    p_glUniform2f(locResolution, (GLfloat)w, (GLfloat)h);
    p_glUniform1f(locScale, (GLfloat)scale);
    p_glUniform1i(locMaxIter, MAX_ITERATIONS);

    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
    p_glUseProgram(0);

    glutSwapBuffers();
    glFinish();

    int ms = glutGet(GLUT_ELAPSED_TIME) - t0;
    char title[96];
    std::snprintf(title, sizeof(title), "Mandelbrot GPU - %d ms/frame", ms);
    glutSetWindowTitle(title);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 'Q':
    case 27: // ESC
        exit(0);
        break;
    case 'r':
    case 'R':
        // Сброс к начальному виду
        centerX = -0.5;
        centerY = 0.0;
        scale = 2.0;
        glutPostRedisplay();
        break;
    case '+':
        scale *= 0.8;
        glutPostRedisplay();
        break;
    case '-':
        scale *= 1.2;
        glutPostRedisplay();
        break;
    }
}

void specialKeys(int key, int x, int y) {
    double moveStep = 0.1 * scale;

    switch (key) {
    case GLUT_KEY_UP:
        centerY += moveStep;
        break;
    case GLUT_KEY_DOWN:
        centerY -= moveStep;
        break;
    case GLUT_KEY_LEFT:
        centerX -= moveStep;
        break;
    case GLUT_KEY_RIGHT:
        centerX += moveStep;
        break;
    case GLUT_KEY_PAGE_UP:
        scale *= 0.8;
        break;
    case GLUT_KEY_PAGE_DOWN:
        scale *= 1.2;
        break;
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        double zoomFactor = 2.0;

        if (button == GLUT_LEFT_BUTTON) {
            // Приближение в точку клика
            double real = centerX + (x - WIDTH / 2.0) * scale / WIDTH;
            double imag = centerY + (HEIGHT / 2.0 - y) * scale / HEIGHT;

            centerX = real;
            centerY = imag;
            scale /= zoomFactor;
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            // Отдаление от центра
            scale *= zoomFactor;
        }

        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    if (!initGL()) {
        std::cerr << "Не удалось инициализировать шейдеры." << std::endl;
        exit(1);
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Управление:" << std::endl;
    std::cout << "Стрелки - перемещение" << std::endl;
    std::cout << "Page Up/Down - приближение/отдаление" << std::endl;
    std::cout << "+/- - приближение/отдаление" << std::endl;
    std::cout << "Левый клик - приближение в точку" << std::endl;
    std::cout << "Правый клик - отдаление" << std::endl;
    std::cout << "R - сброс к начальному виду" << std::endl;
    std::cout << "Q или ESC - выход" << std::endl;
}

int main(int argc, char** argv) {
    SetConsoleOutputCP(65001);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Mandelbrot GPU");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}
