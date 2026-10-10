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
endif() 

# Поиск пакетов
find_package(OpenGL REQUIRED)

# Пути к freeglut
set(FREEGLUT_DIR "D:/programming/libs/freeglut")
set(FREEGLUT_INCLUDE_DIR "${FREEGLUT_DIR}/include")
set(FREEGLUT_LIB_DIR "${FREEGLUT_DIR}/lib/x64")

# Проверка существования путей
if(NOT EXISTS ${FREEGLUT_INCLUDE_DIR})
    message(WARNING "FreeGLUT include directory not found: ${FREEGLUT_INCLUDE_DIR}")
endif()

if(NOT EXISTS ${FREEGLUT_LIB_DIR})
    message(WARNING "FreeGLUT lib directory not found: ${FREEGLUT_LIB_DIR}")
endif()

# Включение директорий
include_directories(${FREEGLUT_INCLUDE_DIR})
link_directories(${FREEGLUT_LIB_DIR})

# Добавление исполняемого файла
add_executable(MandelbrotFractal Mandelbrot.cpp)

# Настройка целевых свойств
target_include_directories(MandelbrotFractal PRIVATE ${FREEGLUT_INCLUDE_DIR})
target_link_directories(MandelbrotFractal PRIVATE ${FREEGLUT_LIB_DIR})

# Связывание библиотек
target_link_libraries(MandelbrotFractal 
    PRIVATE 
    OpenGL::GL
    ${FREEGLUT_LIB_DIR}/freeglut.lib
    opengl32.lib
    glu32.lib
    winmm.lib
)

# Для отладки: вывод информации о путях
message(STATUS "FreeGLUT include dir: ${FREEGLUT_INCLUDE_DIR}")
message(STATUS "FreeGLUT lib dir: ${FREEGLUT_LIB_DIR}")

# Копирование DLL файлов в выходную директорию
add_custom_command(TARGET MandelbrotFractal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FREEGLUT_DIR}/bin/x64/freeglut.dll"
        $<TARGET_FILE_DIR:MandelbrotFractal>
    COMMENT "Copying FreeGLUT DLL to output directory"
)

# Настройка свойств Visual Studio
if(MSVC)
    # Настройка для отладки
    target_compile_definitions(MandelbrotFractal PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
    
    # Отключение предупреждений для безопасных функций
    target_compile_options(MandelbrotFractal PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd4996>)
endif()

# Дополнительные настройки для Windows
if(WIN32)
    target_link_libraries(MandelbrotFractal PRIVATE gdi32)
endif()