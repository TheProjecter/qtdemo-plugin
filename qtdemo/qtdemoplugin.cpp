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

#include "qtdemoplugin.h"

#include "qtdemomode.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/basemode.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <QtCore/QDebug>
#include <QtCore/QtPlugin>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

using namespace QtDemo::Internal;

QtDemoPlugin::QtDemoPlugin()
  : m_qtdemoMode(0), m_communityQtDemoPage(0)
{
}

QtDemoPlugin::~QtDemoPlugin()
{
    if (m_qtdemoMode) {
        removeObject(m_qtdemoMode);
        delete m_qtdemoMode;
    }
//    if (m_communityQtDemoPage) {
//        removeObject(m_communityQtDemoPage);
//        delete m_communityQtDemoPage;
//    }
}

/*! Initializes the plugin. Returns true on success.
    Plugins want to register objects with the plugin manager here.

    \a error_message can be used to pass an error message to the plugin system,
       if there was any.
*/
bool QtDemoPlugin::initialize(const QStringList &arguments, QString *error_message)
{
    Q_UNUSED(arguments)
    Q_UNUSED(error_message)

//    m_communityQtDemoPage = new Internal::CommunityQtDemoPage;
//    addObject(m_communityQtDemoPage);

    m_qtdemoMode = new QtDemoMode;
    addObject(m_qtdemoMode);

    return true;
}

/*! Notification that all extensions that this plugin depends on have been
    initialized. The dependencies are defined in the plugins .qwp file.

    Normally this method is used for things that rely on other plugins to have
    added objects to the plugin manager, that implement interfaces that we're
    interested in. These objects can now be requested through the
    PluginManagerInterface.

    The QtDemoPlugin doesn't need things from other plugins, so it does
    nothing here.
*/
void QtDemoPlugin::extensionsInitialized()
{
    m_qtdemoMode->initPlugins();
//    Core::ModeManager::instance()->activateMode(m_qtdemoMode->id());
}

Q_EXPORT_PLUGIN(QtDemoPlugin)
