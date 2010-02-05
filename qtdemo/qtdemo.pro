CONFIG += assistant help x11inc
TEMPLATE = lib
TARGET = QtDemo
QT += xml network

contains(QT_CONFIG, opengl) {
    DEFINES += QT_OPENGL_SUPPORT
    QT += opengl
}

include(../../qtcreatorplugin.pri)
include(qtdemo_dependencies.pri)
HEADERS += qtdemoplugin.h \
    qtdemomode.h \
    qtdemo_global.h \
    mainwindow.h \
    demoscene.h \
    demoitem.h \
    score.h \
    demoitemanimation.h \
    itemcircleanimation.h \
    demotextitem.h \
    headingitem.h \
    dockitem.h \
    scanitem.h \
    letteritem.h \
    examplecontent.h \
    menucontent.h \
    guide.h \
    guideline.h \
    guidecircle.h \
    menumanager.h \
    colors.h \
    textbutton.h \
    imageitem.h
#    qtoptionspage.h
SOURCES += qtdemoplugin.cpp \
    qtdemomode.cpp \
#	main.cpp \
    demoscene.cpp \
    mainwindow.cpp \
    demoitem.cpp \
    score.cpp \
    demoitemanimation.cpp \
    itemcircleanimation.cpp \
    demotextitem.cpp \
    headingitem.cpp \
    dockitem.cpp \
    scanitem.cpp \
    letteritem.cpp \
    examplecontent.cpp \
    menucontent.cpp \
    guide.cpp \
    guideline.cpp \
    guidecircle.cpp \
    menumanager.cpp \
    colors.cpp \
    textbutton.cpp \
    imageitem.cpp
#    qtoptionspage.cpp

FORMS += qtdemomode.ui
#    communityqtdemopagewidget.ui
RESOURCES += qtdemo.qrc images.qrc
DEFINES += QTDEMO_LIBRARY
OTHER_FILES += qtdemo.pluginspec

win32:RC_FILE = qtdemo.rc
mac {
ICON = qtdemo.icns
QMAKE_INFO_PLIST = Info_mac.plist
}
