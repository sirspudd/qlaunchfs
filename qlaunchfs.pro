QT += gui gui-private core-private waylandcompositor waylandcompositor-private

HEADERS += \
    window.h \
    compositor.h

SOURCES += main.cpp \
    window.cpp \
    compositor.cpp

TARGET = qlaunchfs
