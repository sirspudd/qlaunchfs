QT += gui gui-private core-private waylandcompositor waylandcompositor-private

HEADERS += \
    window.h \
    compositor.h

SOURCES += main.cpp \
    window.cpp \
    compositor.cpp

target.path = qlaunchfs
INSTALLS += target
