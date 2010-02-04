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
    MainWindow* m_mainWindow;
    QMap<QAbstractButton*, QWidget*> buttonMap;
    Ui::QtDemoMode ui;
    int currentTip;
};

QtDemoModePrivate::QtDemoModePrivate()
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

    m_d->m_mainWindowHolder = new QWidget(m_d->m_widget);
    m_d->ui.setupUi(m_d->m_mainWindowHolder);
    l->addWidget(m_d->m_mainWindowHolder);

    m_d->m_scrollArea = new QScrollArea;
    m_d->m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_d->m_scrollArea->setWidget(m_d->m_widget);
    m_d->m_scrollArea->setWidgetResizable(true);
}

QtDemoMode::~QtDemoMode()
{
    delete m_d->m_widget;
    delete m_d;
}

QString QtDemoMode::displayName() const
{
    return tr("QtDemo");
}

QIcon QtDemoMode::icon() const
{
    return QIcon(QLatin1String(":/qtdemo/images/qt_logo.png"));
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

QString QtDemoMode::id() const
{
    return QLatin1String(MODE_QTDEMO);
}

QList<int> QtDemoMode::context() const
{
    static QList<int> contexts = QList<int>()
                                 << Core::UniqueIDManager::instance()->uniqueIdentifier(/*Core::Constants::*/C_QTDEMO_MODE);
    return contexts;
}

void QtDemoMode::initPlugins()
{
    delete m_d->ui.stackedWidget->currentWidget();

//    Q_INIT_RESOURCE(qtdemo);
    m_d->m_mainWindow = new MainWindow(m_d->ui.stackedWidget);
    m_d->ui.stackedWidget->addWidget(m_d->m_mainWindow);
    MenuManager::instance()->init(m_d->m_mainWindow);

    artisticSleep(500);
    m_d->m_mainWindow->start();
}

} // namespace QtDemo
