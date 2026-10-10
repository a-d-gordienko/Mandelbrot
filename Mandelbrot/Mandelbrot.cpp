//#include <GL/freeglut.h>
//#include <complex>
//#include <cmath>
//#include <iostream>
//#include <thread>
//
//// Параметры отображения
//const int WIDTH = 800;
//const int HEIGHT = 800;
//const int MAX_ITERATIONS = 1000;
//
//// Параметры фрактала
//double centerX = -0.5;
//double centerY = 0.0;
//double scale = 2.0;
//
//// Цветовая палитра
//struct Color {
//    float r, g, b;
//};
//
//Color getColor(int iterations, int maxIterations) {
//    Color color;
//
//    if (iterations == maxIterations) {
//        // Точки внутри множества - черные
//        color.r = color.g = color.b = 0.0f;
//    }
//    else {
//        // Разноцветные точки снаружи множества
//        float t = (float)iterations / maxIterations;
//
//        // Цветовая схема: радужная
//        color.r = 0.5f * sin(t * 6.28318f) + 0.5f;
//        color.g = 0.5f * sin(t * 6.28318f + 2.09439f) + 0.5f;
//        color.b = 0.5f * sin(t * 6.28318f + 4.18879f) + 0.5f;
//    }
//
//    return color;
//}
//
//int mandelbrot(double real, double imag, int maxIterations) {
//    std::complex<double> c(real, imag);
//    std::complex<double> z(0.0, 0.0);
//
//    int iterations = 0;
//
//    while (iterations < maxIterations) {
//        z = z * z + c;
//
//        // Если точка ушла за радиус 2, она не принадлежит множеству
//        if (std::abs(z) > 2.0) {
//            break;
//        }
//
//        iterations++;
//    }
//
//    return iterations;
//}
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT);
//    glBegin(GL_POINTS);
//
//    // Проходим по всем пикселям окна
//    for (int y = 0; y < HEIGHT; y++) {
//        for (int x = 0; x < WIDTH; x++) {
//            // Преобразуем координаты пикселя в координаты комплексной плоскости
//            double real = centerX + (x - WIDTH / 2.0) * scale / WIDTH;
//            double imag = centerY + (y - HEIGHT / 2.0) * scale / HEIGHT;
//
//            // Вычисляем количество итераций для точки
//            int iterations = mandelbrot(real, imag, MAX_ITERATIONS);
//            // Получаем цвет
//            Color color = getColor(iterations, MAX_ITERATIONS);
//
//            // Устанавливаем цвет и рисуем точку
//            glColor3f(color.r, color.g, color.b);
//            glVertex2i(x, y);
//        }
//    }
//
//    glEnd();
//    glutSwapBuffers();
//}
//
//void reshape(int w, int h) {
//    glViewport(0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0, w, 0, h);
//    glMatrixMode(GL_MODELVIEW);
//}
//
//void keyboard(unsigned char key, int x, int y) {
//    switch (key) {
//    case 'q':
//    case 'Q':
//    case 27: // ESC
//        exit(0);
//        break;
//    case 'r':
//    case 'R':
//        // Сброс к начальному виду
//        centerX = -0.5;
//        centerY = 0.0;
//        scale = 2.0;
//        glutPostRedisplay();
//        break;
//    case '+':
//        scale *= 0.8;
//        glutPostRedisplay();
//        break;
//    case '-':
//        scale *= 1.2;
//        glutPostRedisplay();
//        break;
//    }
//}
//
//void specialKeys(int key, int x, int y) {
//    double moveStep = 0.1 * scale;
//
//    switch (key) {
//    case GLUT_KEY_UP:
//        centerY += moveStep;
//        break;
//    case GLUT_KEY_DOWN:
//        centerY -= moveStep;
//        break;
//    case GLUT_KEY_LEFT:
//        centerX -= moveStep;
//        break;
//    case GLUT_KEY_RIGHT:
//        centerX += moveStep;
//        break;
//    case GLUT_KEY_PAGE_UP:
//        // Приближение
//        scale *= 0.8;
//        break;
//    case GLUT_KEY_PAGE_DOWN:
//        // Отдаление
//        scale *= 1.2;
//        break;
//    }
//
//    glutPostRedisplay();
//}
//
//void mouse(int button, int state, int x, int y) {
//    if (state == GLUT_DOWN) {
//        double zoomFactor = 2.0;
//
//        if (button == GLUT_LEFT_BUTTON) {
//            // Приближение в точку клика
//            double real = centerX + (x - WIDTH / 2.0) * scale / WIDTH;
//            double imag = centerY + (HEIGHT / 2.0 - y) * scale / HEIGHT;
//
//            centerX = real;
//            centerY = imag;
//            scale /= zoomFactor;
//        }
//        else if (button == GLUT_RIGHT_BUTTON) {
//            // Отдаление от центра
//            scale *= zoomFactor;
//        }
//
//        glutPostRedisplay();
//    }
//}
//
//void init() {
//    glClearColor(0.0, 0.0, 0.0, 1.0);
//    glPointSize(1.0);
//
//    std::cout << "Управление:" << std::endl;
//    std::cout << "Стрелки - перемещение" << std::endl;
//    std::cout << "Page Up/Down - приближение/отдаление" << std::endl;
//    std::cout << "+/- - приближение/отдаление" << std::endl;
//    std::cout << "Левый клик - приближение в точку" << std::endl;
//    std::cout << "Правый клик - отдаление" << std::endl;
//    std::cout << "R - сброс к начальному виду" << std::endl;
//    std::cout << "Q или ESC - выход" << std::endl;
//}
//
//int main(int argc, char** argv) {
//    SetConsoleOutputCP(1251);
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//    glutInitWindowSize(WIDTH, HEIGHT);
//    glutInitWindowPosition(100, 100);
//    glutCreateWindow("Фрактал Мандельброта - Visual Studio 2022");
//
//    init();
//
//    glutDisplayFunc(display);
//    glutReshapeFunc(reshape);
//    glutKeyboardFunc(keyboard);
//    glutSpecialFunc(specialKeys);
//    glutMouseFunc(mouse);
//
//    glutMainLoop();
//
//    return 0;
//}
#include <GL/freeglut.h>

#include <cmath>
#include <iostream>

// Параметры отображения
const int WIDTH = 800;
const int HEIGHT = 800;
const int MAX_ITERATIONS = 1000;

// Параметры фрактала
double centerX = -0.5;
double centerY = 0.0;
double scale = 2.0;

// Цветовая палитра
struct Color {
    float r, g, b;
};

// Оптимизированная функция получения цвета
Color getColor(int iterations, int maxIterations) {
    Color color;

    if (iterations == maxIterations) {
        // Точки внутри множества - черные
        color.r = color.g = color.b = 0.0f;
    }
    else {
        // Оптимизированная цветовая схема
        float t = (float)iterations / maxIterations;

        // Более быстрая цветовая схема без тригонометрических функций
        color.r = t * 4.0f;
        color.g = t * 2.0f;
        color.b = t * 1.0f;

        // Нормализация значений
        if (color.r > 1.0f) color.r = 2.0f - color.r;
        if (color.g > 1.0f) color.g = 2.0f - color.g;
        if (color.b > 1.0f) color.b = 2.0f - color.b;
    }

    return color;
}

// Проверка принадлежности точки главной кардиоиде
bool isInCardioid(double real, double imag) {
    // Оптимизированная проверка на кардиоиду
    double q = (real - 0.25) * (real - 0.25) + imag * imag;
    return q * (q + (real - 0.25)) <= 0.25 * imag * imag;
}

// Проверка принадлежности точке период-2 бульбе
bool isInBulb(double real, double imag) {
    double x1 = real + 1.0;
    return x1 * x1 + imag * imag <= 0.0625; // 1/16 = 0.0625
}

// Оптимизированная функция Мандельброта без std::complex
int mandelbrot(double real, double imag, int maxIterations) {
    // Быстрая проверка на принадлежность к основным внутренним областям
    if (isInCardioid(real, imag) || isInBulb(real, imag)) {
        return maxIterations;
    }

    // Оптимизированный расчет с использованием примитивных типов
    double x = 0.0, y = 0.0;
    double x2 = 0.0, y2 = 0.0;
    int iterations = 0;

    // Оптимизация: используем квадраты для избежания вычисления квадратного корня
    while (iterations < maxIterations) {
        // Вычисляем новые значения
        y = 2 * x * y + imag;
        x = x2 - y2 + real;

        // Квадраты для следующей итерации
        x2 = x * x;
        y2 = y * y;

        // Проверка условия выхода
        if (x2 + y2 > 4.0) {
            break;
        }

        iterations++;
    }

    return iterations;
}

// Оптимизированная функция отрисовки с предварительными вычислениями
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);

    // Предварительные вычисления констант
    double scaleFactor = scale / WIDTH;
    double halfWidth = WIDTH / 2.0;
    double halfHeight = HEIGHT / 2.0;

    // Проходим по всем пикселям окна
    for (int y = 0; y < HEIGHT; y++) {
        // Предварительное вычисление imag для всей строки
        double imag = centerY + (y - halfHeight) * scaleFactor;

        for (int x = 0; x < WIDTH; x++) {
            // Преобразуем координаты пикселя в координаты комплексной плоскости
            double real = centerX + (x - halfWidth) * scaleFactor;

            // Вычисляем количество итераций для точки
            int iterations = mandelbrot(real, imag, MAX_ITERATIONS);

            // Получаем цвет
            Color color = getColor(iterations, MAX_ITERATIONS);

            // Устанавливаем цвет и рисуем точку
            glColor3f(color.r, color.g, color.b);
            glVertex2i(x, y);
        }
    }

    glEnd();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
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
    glPointSize(1.0);

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
    SetConsoleOutputCP(1251);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Оптимизированный фрактал Мандельброта");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}