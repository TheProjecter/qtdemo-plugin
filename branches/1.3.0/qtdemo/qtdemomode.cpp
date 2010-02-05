/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#include "qtdemomode.h"
#include <extensionsystem/pluginmanager.h>

#include <coreplugin/icore.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/dialogs/newdialog.h>

#include <qt4projectmanager/qtversionmanager.h>

#include <utils/styledbar.h>
#include <utils/welcomemodetreewidget.h>
#include <utils/iwelcomepage.h>

#include <QtGui/QMouseEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QButtonGroup>
#include <QtGui/QDesktopServices>
#include <QtGui/QToolButton>

#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <cstdlib>

#include "ui_qtdemomode.h"
#include "mainwindow.h"
#include "menumanager.h"

const char * const MODE_QTDEMO           = "QtDemo";
const int          P_MODE_QTDEMO         = 5;
const char * const C_QTDEMO_MODE        = "Core.QtDemoMode";

using namespace ExtensionSystem;
using namespace Utils;
//using namespace Qt4ProjectManager::Internal;
using namespace Qt4ProjectManager;

namespace QtDemo {

    static void artisticSleep(int sleepTime)
    {
        QTime time;
        time.restart();
        while (time.elapsed() < sleepTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    }


struct QtDemoModePrivate
{
    QtDemoModePrivate();

    QScrollArea *m_scrollArea;
    QWidget *m_widget;
    QWidget* m_mainWindowHolder;
    QWidget* m_fullScreenWindow;
    MainWindow* m_mainWindow;
    QMap<QAbstractButton*, QWidget*> buttonMap;
    Ui::QtDemoMode ui;
    int currentTip;
};

QtDemoModePrivate::QtDemoModePrivate() : m_mainWindowHolder(0), m_fullScreenWindow(0)
{
}

// ---  QtDemoMode
QtDemoMode::QtDemoMode() :
    m_d(new QtDemoModePrivate)
{
    m_d->m_widget = new QWidget;
    QVBoxLayout *l = new QVBoxLayout(m_d->m_widget);
    l->setMargin(0);
    l->setSpacing(0);
    l->addWidget(new Utils::StyledBar(m_d->m_widget));

    m_d->m_scrollArea = new QScrollArea();
    m_d->m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_d->m_scrollArea->setWidget(m_d->m_widget);
    m_d->m_scrollArea->setWidgetResizable(true);

//    PluginManager *pluginManager = PluginManager::instance();
//    connect(pluginManager, SIGNAL(objectAdded(QObject*)), SLOT(projectManagerAdded(QObject*)));
}

QtDemoMode::~QtDemoMode()
{
    delete m_d->m_widget;
    delete m_d->m_scrollArea;
    delete m_d;
}

//QString QtDemoMode::displayName() const
//{
//    return tr("QtDemo");
//}

QString QtDemoMode::name() const
{
    return tr("QtDemo");
}

QIcon QtDemoMode::icon() const
{
    return QIcon(QLatin1String(":/qtdemo/images/qt_logo.png"));
}

const char* QtDemoMode::uniqueModeName() const
{
    return /*Core::Constants::*/MODE_QTDEMO;
}

int QtDemoMode::priority() const
{
    // TODO: move to Core::Constants namepsace?
    return /*Core::Constants::*/P_MODE_QTDEMO;
}

QWidget* QtDemoMode::widget()
{
    return m_d->m_scrollArea;
}

//QString QtDemoMode::id() const
//{
//    return QLatin1String(MODE_QTDEMO);
//}

QList<int> QtDemoMode::context() const
{
    static QList<int> contexts = QList<int>()
                                 << Core::UniqueIDManager::instance()->uniqueIdentifier(/*Core::Constants::*/C_QTDEMO_MODE);
    return contexts;
}

void QtDemoMode::initPlugins()
{
    this->resolveSdkHome();
#if 1
    m_d->m_mainWindowHolder = new QWidget(m_d->m_widget);
    m_d->ui.setupUi(m_d->m_mainWindowHolder);
    m_d->m_widget->layout()->addWidget(m_d->m_mainWindowHolder);

    delete m_d->ui.stackedWidget->currentWidget();
//    Q_INIT_RESOURCE(qtdemo);
    m_d->m_mainWindow = new MainWindow(m_d->ui.stackedWidget);
    m_d->ui.stackedWidget->addWidget(m_d->m_mainWindow);
#else
    m_d->m_mainWindow = new MainWindow(m_d->m_widget);
    m_d->m_mainWindowHolder = m_d->m_mainWindow;
    m_d->m_widget->layout()->addWidget(m_d->m_mainWindowHolder);
#endif
    MenuManager::instance()->init(m_d->m_mainWindow);
#if 0
    // QtVersionManager is not exported in qt4projectmanager plugin project
    QtVersionManager *mgr = PluginManager::instance()->getObject<QtVersionManager>();
    if (mgr)
//    QtVersionManager *mgr = QtVersionManager::instance();
    connect(mgr, SIGNAL(updateExamples(QString,QString,QString)),
            m_d->m_mainWindow, SLOT(updateExamples(QString,QString,QString)));
#endif
#if 1
    //    Core::ICore::instance()->resourcePath();
    if (!MenuManager::instance()->sdkHome().isEmpty())
    {
        artisticSleep(500);
        m_d->m_mainWindow->start();

        connect(MenuManager::instance(), SIGNAL(toggleFullScreen()), this, SLOT(toggleFullScreen()));
    }
#endif
}

void QtDemoMode::resolveSdkHome()
{
//    MenuManager::instance()->qtSdkHome = QLatin1String("c:/Qt/4.6.0");
    QString propertiesFileName = QCoreApplication::applicationDirPath() + "/QtDemo.properties";
    QFile propertiesFile(propertiesFileName);
    if (propertiesFile.exists() && propertiesFile.open(QIODevice::ReadOnly))
    {
        QTextStream ts(&propertiesFile);
        while(!ts.atEnd())
        {
            QString line = ts.readLine();
            if (line.startsWith('#'))
                continue;
            if (line.contains(QLatin1String("QT_SDK_HOME")))
            {
                int pos = line.indexOf('=');
                if (pos > 0)
                {
                    line = line.mid(pos+1).trimmed();
                    if (QFile::exists(line))
                    {
                        MenuManager::instance()->qtSdkHome = line;
                    }
                    else
                    {
                        QMessageBox::critical(0,
                                              QObject::tr("QtDemo plugin"),
                                              QObject::tr("Error! QT_SDK_HOME: %1 \nin %2")
                                              .arg(line).arg(propertiesFileName)
                                              );
                    }
                }
                break;
            }
        }
    }
}

//void QtDemoMode::projectManagerAdded(QObject *obj)
//{
//    if (ProjectExplorer::IProjectManager *projectMgr = qobject_cast<ProjectExplorer::IProjectManager*>(obj))
//    {
//        this->m_projectExplorer = projectMgr;
//    }
//}
void QtDemoMode::toggleFullScreen()
{
    if (m_d->ui.stackedWidget->parent() == m_d->ui.mainFrame)
    {
        m_d->m_widget->layout()->removeWidget(m_d->m_mainWindowHolder);
        m_d->ui.mainFrame->layout()->removeWidget(m_d->ui.stackedWidget);
        m_d->m_widget->layout()->addWidget(m_d->ui.stackedWidget);
    }
    else
    {
        m_d->m_widget->layout()->removeWidget(m_d->ui.stackedWidget);
        m_d->ui.mainFrame->layout()->addWidget(m_d->ui.stackedWidget);
        m_d->m_widget->layout()->addWidget(m_d->m_mainWindowHolder);
    }
    m_d->m_mainWindow->setFocus();
}

} // namespace QtDemo
