/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "menumanager.h"
#include "colors.h"
#include "dockitem.h"
#include "demotextitem.h"
#include "imageitem.h"
#include "demoitem.h"
#include "demoscene.h"

#ifndef QT_NO_OPENGL
    #include <QGLWidget>
#endif
//#define QT_NO_OPENGL

MainWindow::MainWindow(QWidget *parent) : QGraphicsView(parent), updateTimer(this)
{
    this->currentFps = Colors::fps;
    this->loop = false;
    this->fpsMedian = -1;
    this->fpsLabel = 0;
    this->pausedLabel = 0;
    this->doneAdapt = false;
    this->useTimer = false;
    this->updateTimer.setSingleShot(true);
    this->companyLogo = 0;
    this->qtLogo = 0;
    this->setupWidget();
    this->setupScene();
    this->setupSceneItems();
    this->drawBackgroundToPixmap();
}

MainWindow::~MainWindow()
{
    delete this->companyLogo;
    delete this->qtLogo;
}

void MainWindow::setupWidget()
{
    QRect screenRect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    QRect windowRect(0, 0, 800, 600);
    if (screenRect.width() < 800)
        windowRect.setWidth(screenRect.width());
    if (screenRect.height() < 600)
        windowRect.setHeight(screenRect.height());
    windowRect.moveCenter(screenRect.center());
    this->setGeometry(windowRect);
    this->setMinimumSize(80, 60);
    setWindowTitle(tr("Qt Examples and Demos"));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    this->setRenderingSystem();
    connect(&this->updateTimer, SIGNAL(timeout()), this, SLOT(tick()));
}

void MainWindow::setRenderingSystem()
{
    QWidget *viewport = 0;

#ifndef QT_NO_OPENGL
    if (Colors::openGlRendering) {
        QGLWidget *glw = new QGLWidget(QGLFormat(QGL::SampleBuffers));
        if (Colors::noScreenSync)
            glw->format().setSwapInterval(0);
        glw->setAutoFillBackground(false);
        viewport = glw;
        setCacheMode(QGraphicsView::CacheNone);
        if (Colors::verbose)
            qDebug() << "- using OpenGL";
    } else // software rendering
#endif
    {
        // software rendering
        viewport = new QWidget;
        setCacheMode(QGraphicsView::CacheBackground);
        if (Colors::verbose)
            qDebug() << "- using software rendering";
    }

    setViewport(viewport);
}

void MainWindow::start()
{
    this->switchTimerOnOff(true);
    this->demoStartTime.restart();
    MenuManager::instance()->itemSelected(MenuManager::ROOT, Colors::rootMenuName);
    if (Colors::verbose)
        qDebug("- starting demo");
}

void MainWindow::enableMask(bool enable)
{
    if (!enable || Colors::noWindowMask)
        this->clearMask();
    else {
        QPolygon region;
        region.setPoints(9,
                                // north side:
                                0, 0,
                                800, 0,
                                // east side:
                                // 800, 70,
                                // 790, 90,
                                // 790, 480,
                                // 800, 500,
                                800, 600,
                                // south side:
                                700, 600,
                                670, 590,
                                130, 590,
                                100, 600,
                                0, 600,
                                // west side:
                                // 0, 550,
                                // 10, 530,
                                // 10, 520,
                                // 0, 520,
                                0, 0);
        this->setMask(QRegion(region));
    }
}

void MainWindow::setupScene()
{
    this->scene = new DemoScene(this);
    this->scene->setSceneRect(0, 0, 800, 600);
    setScene(this->scene);
    this->scene->setItemIndexMethod(QGraphicsScene::NoIndex);
}

void MainWindow::drawItems(QPainter *painter, int numItems, QGraphicsItem **items, const QStyleOptionGraphicsItem* options)
{
    QGraphicsView::drawItems(painter, numItems, items, options);
}

void MainWindow::switchTimerOnOff(bool on)
{
    bool ticker = MenuManager::instance()->ticker && MenuManager::instance()->ticker->scene();
    if (ticker)
        MenuManager::instance()->ticker->tickOnPaint = !on || Colors::noTimerUpdate;

    if (on && !Colors::noTimerUpdate){
        this->useTimer = true;
        this->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
        this->fpsTime = QTime::currentTime();
        this->updateTimer.start(int(1000 / Colors::fps));
    }
    else{
        this->useTimer = false;
        this->updateTimer.stop();
        if (Colors::softwareRendering)
            if (Colors::noTicker)
                this->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
            else
                this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        else
            this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }
}

bool MainWindow::measureFps()
{
    // Calculate time diff:
    float t = this->fpsTime.msecsTo(QTime::currentTime());
    if (t == 0)
        t = 0.01f;
    this->currentFps = (1000.0f / t);
    this->fpsHistory += this->currentFps;
    this->fpsTime = QTime::currentTime();

    // Calculate median:
    int size = this->fpsHistory.size();
    if (size == 10){
        qSort(this->fpsHistory.begin(), this->fpsHistory.end());
        this->fpsMedian = this->fpsHistory.at(int(size/2));
        if (this->fpsMedian == 0)
            this->fpsMedian = 0.01f;
        this->fpsHistory.clear();
        return true;
    }
    return false;
}

/**
    Used for adaption in case things are so slow
    that no median  yet has been calculated
*/
void MainWindow::forceFpsMedianCalculation()
{
    if (this->fpsMedian != -1)
        return;

    int size = this->fpsHistory.size();
    if (size == 0){
        this->fpsMedian = 0.01f;
        return;
    }

    qSort(this->fpsHistory.begin(), this->fpsHistory.end());
    this->fpsMedian = this->fpsHistory.at(int(size/2));
    if (this->fpsMedian == 0)
        this->fpsMedian = 0.01f;
}

void MainWindow::tick()
{
    bool medianChanged = this->measureFps();
    this->checkAdapt();

    if (medianChanged && this->fpsLabel && Colors::showFps)
        this->fpsLabel->setText(QString("FPS: ") + QString::number(int(this->currentFps)));

    if (MenuManager::instance()->ticker)
        MenuManager::instance()->ticker->tick();

    this->viewport()->update();
    if (this->useTimer)
        this->updateTimer.start(int(1000 / Colors::fps));
}

void MainWindow::setupSceneItems()
{
    if (Colors::showFps){
        this->fpsLabel = new DemoTextItem(QString("FPS: --"), Colors::buttonFont(), Qt::white, -1, this->scene, 0, DemoTextItem::DYNAMIC_TEXT);
        this->fpsLabel->setZValue(100);
        this->fpsLabel->setPos(Colors::stageStartX, 600 - QFontMetricsF(Colors::buttonFont()).height() - 5);
    }

    this->companyLogo = new ImageItem(QImage(":/images/trolltech-logo.png"), 1000, 1000, this->scene, 0, true, 0.5f);
    this->qtLogo = new ImageItem(QImage(":/images/qtlogo_small.png"), 1000, 1000, this->scene, 0, true, 0.5f);
    this->companyLogo->setZValue(100);
    this->qtLogo->setZValue(100);
    this->pausedLabel = new DemoTextItem(QString("PAUSED"), Colors::buttonFont(), Qt::white, -1, this->scene, 0);
    this->pausedLabel->setZValue(100);
    QFontMetricsF fm(Colors::buttonFont());
    this->pausedLabel->setPos(Colors::stageWidth - fm.width("PAUSED"), 590 - fm.height());
    this->pausedLabel->setRecursiveVisible(false);
}

void MainWindow::checkAdapt()
{
    if (this->doneAdapt
        || Colors::noTimerUpdate
        || this->demoStartTime.elapsed() < 2000)
       return;

    this->doneAdapt = true;
    this->forceFpsMedianCalculation();
    Colors::benchmarkFps = this->fpsMedian;
    if (Colors::verbose)
        qDebug() << "- benchmark:" << QString::number(Colors::benchmarkFps) << "FPS";

    if (Colors::noAdapt)
        return;

    if (this->fpsMedian < 30){
        if (MenuManager::instance()->ticker && MenuManager::instance()->ticker->scene()){
            this->scene->removeItem(MenuManager::instance()->ticker);
            Colors::noTimerUpdate = true;
            this->switchTimerOnOff(false);
            if (this->fpsLabel)
               this->fpsLabel->setText(QString("FPS: (") + QString::number(this->fpsMedian) + QString(")"));
            if (Colors::verbose)
               qDebug() << "- benchmark adaption: removed ticker (fps < 30)";
        }

        if (this->fpsMedian < 20){
            Colors::noAnimations = true;
            if (Colors::verbose)
               qDebug() << "- benchmark adaption: animations switched off (fps < 20)";
        }

        Colors::adapted = true;
    }
}

int MainWindow::performBenchmark()
{
/*
    QTime time;
    time.restart();
    while (time.elapsed() < 2000)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
*/
    return 0;
}

void MainWindow::drawBackgroundToPixmap()
{
    const QRectF r = this->scene->sceneRect();
    this->background = QPixmap(qRound(r.width()), qRound(r.height()));
    this->background.fill(Qt::black);
    QPainter painter(&this->background);

    if (false && Colors::useEightBitPalette){
        painter.fillRect(r, Colors::sceneBg1);
    } else {
        QImage bg(":/images/demobg.png");
        painter.drawImage(0, 0, bg);
    }
}

void MainWindow::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->drawPixmap(QPoint(0, 0), this->background);
}

void MainWindow::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);
    QGraphicsView::showEvent(event);
}

void MainWindow::toggleFullscreen()
{
    if (this->isFullScreen()){
        this->enableMask(true);
        this->showNormal();
        if (MenuManager::instance()->ticker)
            MenuManager::instance()->ticker->pause(false);
    }
    else {
        this->enableMask(false);
        this->showFullScreen();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape){
        // disable escape key press
//        this->loop = false;
//        QApplication::quit();
    }
    else if (event->key() == Qt::Key_1){
            QString s("");
            s += "Rendering system: ";
            if (Colors::openGlRendering)
                s += "OpenGL";
            else
                s += "software";

            s += "\nAdapt: ";
            s += Colors::noAdapt ? "off" : "on";
            s += "\nAdaption occured: ";
            s += Colors::adapted ? "yes" : "no";
            s += "\nOpenGL version: ";
            s += Colors::glVersion;
            QWidget w;
            s += "\nColor bit depth: ";
            s += QString::number(w.depth());
            s += "\nWanted FPS: ";
            s += QString::number(Colors::fps);
            s += "\nBenchmarked FPS: ";
            s += Colors::benchmarkFps != -1 ? QString::number(Colors::benchmarkFps) : "not calculated";
            s += "\nAnimations: ";
            s += Colors::noAnimations ? "off" : "on";
            s += "\nBlending: ";
            s += Colors::useEightBitPalette ? "off" : "on";
            s += "\nTicker: ";
            s += Colors::noTicker ? "off" : "on";
            s += "\nPixmaps: ";
            s += Colors::usePixmaps ? "on" : "off";
            s += "\nRescale images on resize: ";
            s += Colors::noRescale ? "off" : "on";
            s += "\nTimer based updates: ";
            s += Colors::noTimerUpdate ? "off" : "on";
            s += "\nSeparate loop: ";
            s += Colors::useLoop ? "yes" : "no";
            s += "\nScreen sync: ";
            s += Colors::noScreenSync ? "no" : "yes";
            QMessageBox::information(0, QString("Current configuration"), s);
    }
}

void MainWindow::focusInEvent(QFocusEvent *)
{
    if (!Colors::pause)
        return;

    if (MenuManager::instance()->ticker)
        MenuManager::instance()->ticker->pause(false);

    int code = MenuManager::instance()->currentMenuCode;
    if (code == MenuManager::ROOT || code == MenuManager::MENU1)
        this->switchTimerOnOff(true);

    this->pausedLabel->setRecursiveVisible(false);
}

void MainWindow::focusOutEvent(QFocusEvent *)
{
    if (!Colors::pause)
        return;

    if (MenuManager::instance()->ticker)
        MenuManager::instance()->ticker->pause(true);

    int code = MenuManager::instance()->currentMenuCode;
    if (code == MenuManager::ROOT || code == MenuManager::MENU1)
        this->switchTimerOnOff(false);

    this->pausedLabel->setRecursiveVisible(true);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    this->resetMatrix();
    this->scale(event->size().width() / 800.0, event->size().height() / 600.0);
    QGraphicsView::resizeEvent(event);
    DemoItem::setMatrix(this->matrix());

    if (this->companyLogo){
        const QRectF r = this->scene->sceneRect();
        QRectF ttb = this->companyLogo->boundingRect();
        this->companyLogo->setPos(int((r.width() - ttb.width()) / 2), 595 - ttb.height());
        QRectF qtb = this->qtLogo->boundingRect();
        this->qtLogo->setPos(802 - qtb.width(), 0);
    }

    // Changing size will almost always
    // hurt FPS during the changing. So
    // ignore it.
    this->fpsHistory.clear();
}

void MainWindow::updateExamples(const QString& examplePath, const QString& demosPath, const QString &sourcePath)
{
    QMessageBox mbox(this);
    QStringList slist;
    slist << "examplePath:" << examplePath << "\n"
            << "demosPath:" << demosPath << "\n"
            << "sourcePath:" << sourcePath;

    mbox.setText(slist.join(""));
    mbox.exec();

    QString demoxml = demosPath + "/qtdemo/xml/examples.xml";
    if (!QFile::exists(demoxml)) {
        demoxml = sourcePath + "/demos/qtdemo/xml/examples.xml";
        if (!QFile::exists(demoxml))
            return;
    }

    QFile description(demoxml);
    if (!description.open(QFile::ReadOnly))
        return;

#if 0
    ui->examplesComboBox->clear();
    ui->examplesComboBox->setEnabled(true);

    ui->examplesComboBox->addItem(tr("Choose an example..."));
    QFont f = font();
    f.setItalic(true);
    ui->examplesComboBox->setItemData(0, f, Qt::FontRole);
    f.setItalic(false);
    bool inExamples = false;
    QString dirName;
    QXmlStreamReader reader(&description);
    while (!reader.atEnd()) {
        switch (reader.readNext()) {
            case QXmlStreamReader::StartElement:
            if (reader.name() == "category") {
                QString name = reader.attributes().value(QLatin1String("name")).toString();
                if (name.contains("tutorial"))
                    break;
                dirName = reader.attributes().value(QLatin1String("dirname")).toString();
                ui->examplesComboBox->addItem(name);
                f.setBold(true);
                ui->examplesComboBox->setItemData(ui->examplesComboBox->count()-1, f, Qt::FontRole);
                f.setBold(false);
                inExamples = true;
            }
            if (inExamples && reader.name() == "example") {
                QString name = reader.attributes().value(QLatin1String("name")).toString();
                QString fn = reader.attributes().value(QLatin1String("filename")).toString();
                QString relativeProPath = '/' + dirName + '/' + fn + '/' + fn + ".pro";
                QString fileName = examplePath + relativeProPath;
                if (!QFile::exists(fileName))
                    fileName = sourcePath + "/examples" + relativeProPath;
                QString helpPath = "qthelp://com.trolltech.qt/qdoc/" + dirName.replace("/", "-") + "-" + fn + ".html";

                ui->examplesComboBox->addItem("  " + name, fileName);
                ui->examplesComboBox->setItemData(ui->examplesComboBox->count()-1, helpPath, Qt::UserRole+1);
            }
            break;
            case QXmlStreamReader::EndElement:
            if (reader.name() == "category")
                inExamples = false;
            break;
            default:
            break;
        }
    }
#endif
}
