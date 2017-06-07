/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Wayland module
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QProcess>

#include "window.h"
#include "compositor.h"

int main(int argc, char *argv[])
{
    if (qgetenv("WAYLAND_DISPLAY").isNull()) {
        qputenv("QT_QPA_PLATFORM", "eglfs");
    }

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setAlphaBufferSize(0);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(argc, argv);

    QStringList args = QGuiApplication::arguments();
    // Get rid of argv[0]
    args.pop_front();

    if (args.length() == 0) {
        qDebug() << "This command runs a Qt application passed as an argument";
    } else {
        Window window;
        Compositor compositor(&window);
        window.setCompositor(&compositor);
        compositor.create();
        window.showFullScreen();

        app.processEvents();

        QProcess process;
        QObject::connect(&process, &QProcess::errorOccurred,
              [](QProcess::ProcessError error) {
            qDebug() << "Child process hit the following error:" << error;
            QGuiApplication::exit(-1);
        });
        QObject::connect(&process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
              [](int exitCode, QProcess::ExitStatus exitStatus) {
            qDebug() << "Child process exited with" << exitStatus;
            QGuiApplication::exit(exitCode);
        });

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        // Nested testing
        if (env.keys().contains("WAYLAND_DISPLAY"))
            env.insert("WAYLAND_DISPLAY", "wayland-1");
        //env.insert("WAYLAND_DISPLAY", "wayland-1");
        // Why doesn't this work as expected?
        env.insert("QT_QPA_PLUGIN", "wayland");
        process.setProcessEnvironment(env);

        // Clearly this assumes the item being launched propagates the flag
        args << "-platform wayland";
        QString launchLine = args.join(" ");

        qDebug() << "launching" << launchLine;
        process.start(launchLine);

        return app.exec();
    }
    return -1;
}
