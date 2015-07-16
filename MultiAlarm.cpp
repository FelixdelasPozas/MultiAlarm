/*
 File: MultiAlarm.cpp
 Created on: 02/07/2015
 Author: Felix de las Pozas Alvarez

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Project
#include "MultiAlarm.h"
#include "AboutDialog.h"
#include "NewAlarmDialog.h"
#include "Alarm.h"

// Qt
#include <QCloseEvent>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QDebug>

// C++
#include <iostream>

const QString STATE = "State";
const QString GEOMETRY = "Geometry";

//-----------------------------------------------------------------
MultiAlarm::MultiAlarm(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow{parent, flags}
, m_icon     {new QSystemTrayIcon(QIcon(":/MultiAlarm/application.ico"), this)}
{
  setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
  setupUi(this);

  statusbar->setVisible(false);
  setFixedHeight(m_newButton->height() + menubar->height());
  centralWidget()->layout()->setContentsMargins(0,0,0,0);
  centralWidget()->layout()->setSpacing(0);
  centralWidget()->layout()->setMargin(0);

  restoreSettings();

  setupTrayIcon();

  connectSignals();
}

//-----------------------------------------------------------------
MultiAlarm::~MultiAlarm()
{
  saveSettings();
}

//-----------------------------------------------------------------
void MultiAlarm::createNewAlarm()
{
  QStringList invalidNames, invalidColors;
  for(auto alarm: m_alarms)
  {
    invalidNames << alarm->name();
    invalidColors << alarm->color();
  }

  NewAlarmDialog dialog(invalidNames, invalidColors, this);

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
  {
    auto alarmWidget = new AlarmWidget();

    initWidget(alarmWidget, dialog);

    centralWidget()->layout()->addWidget(alarmWidget);
    setFixedHeight(size().height() + alarmWidget->size().height());

    m_alarms << alarmWidget;
  }
}

//-----------------------------------------------------------------
void MultiAlarm::aboutDialog()
{
  AboutDialog dialog(this);

  dialog.exec();
}

//-----------------------------------------------------------------
void MultiAlarm::changeEvent(QEvent* e)
{
  if (e->type() == QEvent::WindowStateChange)
  {
    if (isMinimized())
    {
      hide();

      m_icon->show();
      e->ignore();
    }
  }
}

//-----------------------------------------------------------------
void MultiAlarm::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if(reason == QSystemTrayIcon::DoubleClick)
  {
    m_icon->hide();
    showNormal();
  }
}

//-----------------------------------------------------------------
void MultiAlarm::setupTrayIcon()
{
  auto menu = new QMenu();
  auto restore = new QAction(tr("Restore"), this);
  auto quit = new QAction(tr("Quit"), this);

  menu->addAction(restore);
  menu->addSeparator();
  menu->addAction(quit);

  connect(restore, SIGNAL(triggered()),
          this,    SLOT(onRestoreActionActivated()));

  connect(quit, SIGNAL(triggered()),
          this, SLOT(onQuitActionActivated()));

  m_icon->setContextMenu(menu);
  m_icon->setToolTip(tr("MultiAlarm"));
  m_icon->hide();
}

//-----------------------------------------------------------------
void MultiAlarm::onRestoreActionActivated()
{
  m_icon->hide();
  showNormal();
}

//-----------------------------------------------------------------
void MultiAlarm::onQuitActionActivated()
{
  m_icon->hide();
  close();
}

//-----------------------------------------------------------------
void MultiAlarm::restoreSettings()
{
  QSettings settings("MultiAlarm.ini", QSettings::IniFormat);

  if(settings.contains(STATE))
  {
    auto state = settings.value(STATE).toByteArray();
    restoreState(state);
  }

  if(settings.contains(GEOMETRY))
  {
    auto geometry = settings.value(GEOMETRY).toByteArray();
    restoreGeometry(geometry);
  }
}

//-----------------------------------------------------------------
void MultiAlarm::saveSettings()
{
  QSettings settings("MultiAlarm.ini", QSettings::IniFormat);

  settings.setValue(STATE, saveState());
  settings.setValue(GEOMETRY, saveGeometry());

  // TODO: save alarms
}

//-----------------------------------------------------------------
void MultiAlarm::initWidget(AlarmWidget *widget, const NewAlarmDialog& dialog)
{
  Alarm *alarm = nullptr;
  auto time = QTime(0,0,0);
  auto dateTime = QDateTime::currentDateTime();

  widget->setColor(dialog.color());
  widget->setName(dialog.name());
  widget->setToolTip(dialog.name() + QString(" Alarm"));

  connect(widget, SIGNAL(deleteAlarm()),
          this,   SLOT(onAlarmDeleted()));

  if(dialog.isTimer())
  {
    auto seconds = time.secsTo(dialog.timerTime());
    time = time.addSecs(seconds);

    alarm = new Alarm(seconds, dialog.timerLoop());
  }
  else
  {
    auto seconds = dateTime.secsTo(dialog.clockDateTime());
    time = time.addSecs(seconds);

    QTime prueba(0,0,0);
    qDebug() << "seconds" << seconds << "entered" << prueba.secsTo(time);
    alarm = new Alarm(seconds, false);
  }

  widget->setTime(time);

  if(!dialog.isTimer())
  {
    alarm->start();
  }

}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmDeleted()
{
  auto widget = qobject_cast<AlarmWidget *>(sender());

  centralWidget()->layout()->removeWidget(widget);
  setFixedHeight(size().height() - widget->size().height());

  m_alarms.removeOne(widget);

  delete widget;
}

//-----------------------------------------------------------------
void MultiAlarm::connectSignals()
{
  connect(m_quitAction, SIGNAL(triggered()),
          this,         SLOT(close()));

  connect(m_newAction, SIGNAL(triggered()),
          this,        SLOT(createNewAlarm()));

  connect(m_newButton, SIGNAL(clicked(bool)),
          this,        SLOT(createNewAlarm()));

  connect(m_aboutAction, SIGNAL(triggered()),
          this,          SLOT(aboutDialog()));

  connect(m_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this,   SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
}
