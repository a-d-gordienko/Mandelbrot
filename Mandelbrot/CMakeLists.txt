#cmake_minimum_required(VERSION 3.15)
#project(MandelbrotFractal)
message("target_name=${target_name}")
# Установка стандарта C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка для Visual Studio
if(MSVC)
    # Отладочная информация и оптимизация
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Предупреждения компилятора
   # add_compile_options(/W4 /WX)
    
    # Отключение некоторых специфических предупреждений
    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/wd4996>)

    # Единая кодировка исходников и строк в бинарнике (UTF-8)
    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
endif() 

# Поиск пакетов
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# GLUT/freeglut: на Linux находится системный freeglut3-dev автоматически.
# На Windows по умолчанию берём локальную сборку freeglut (FREEGLUT_DIR переопределяется через -D).
if(WIN32)
    set(FREEGLUT_DIR "D:/programming/libs/freeglut" CACHE PATH "Локальная сборка FreeGLUT (Windows)")
    add_library(GLUT::GLUT SHARED IMPORTED)
    set_target_properties(GLUT::GLUT PROPERTIES
        IMPORTED_LOCATION "${FREEGLUT_DIR}/bin/x64/freeglut.dll"
        IMPORTED_IMPLIB "${FREEGLUT_DIR}/lib/x64/freeglut.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${FREEGLUT_DIR}/include")
else()
    find_package(GLUT REQUIRED)
endif()

# Добавление исполняемого файла
add_executable(MandelbrotFractal Mandelbrot.cpp)

# Копирование DLL (только Windows)
if(WIN32)
    if(EXISTS "${FREEGLUT_DIR}/bin/x64/freeglut.dll")
        add_custom_command(TARGET MandelbrotFractal POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FREEGLUT_DIR}/bin/x64/freeglut.dll"
                $<TARGET_FILE_DIR:MandelbrotFractal>
            COMMENT "Copying FreeGLUT DLL to output directory"
        )
    endif()
endif()

# Настройка свойств Visual Studio
if(MSVC)
    # Настройка для отладки
    target_compile_definitions(MandelbrotFractal PRIVATE $<$<CONFIG:Debug>:_DEBUG>)

    # Отключение предупреждений для безопасных функций
    target_compile_options(MandelbrotFractal PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd4996>)
endif()

# Связывание библиотек (кросс-платформенное)
target_link_libraries(MandelbrotFractal PRIVATE OpenGL::GL GLUT::GLUT)