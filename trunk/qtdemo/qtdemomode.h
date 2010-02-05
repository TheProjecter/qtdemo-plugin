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

#ifndef QTDEMOMODE_H
#define WELCOMEMODE_H

#include "qtdemo_global.h"

#include <coreplugin/imode.h>

QT_BEGIN_NAMESPACE
class QWidget;
class QUrl;
QT_END_NAMESPACE

namespace QtDemo {

struct QtDemoModePrivate;

class QTDEMO_EXPORT QtDemoMode : public Core::IMode
{
    Q_OBJECT

public:
    QtDemoMode();
    ~QtDemoMode();

    // IMode
//    QString displayName() const;
    virtual QString name() const;
    QIcon icon() const;
    int priority() const;
    QWidget *widget();
//    QString id() const;
    virtual const char *uniqueModeName() const;
    QList<int> context() const;
    void activated();
    QString contextHelpId() const { return QLatin1String("Qt Creator"); }
    void initPlugins();

public Q_SLOTS:
//    void projectManagerAdded(QObject *obj);
    void toggleFullScreen();

private:
    void resolveSdkHome();

private:
    QtDemoModePrivate *m_d;
};

} // namespace QtDemo

#endif // WELCOMEMODE_H
