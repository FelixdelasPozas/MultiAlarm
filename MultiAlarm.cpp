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
  for(auto alarm: m_alarms.keys())
  {
    invalidNames << alarm->name();
    invalidColors << alarm->color();
  }
  qDebug() << invalidNames << invalidColors;

  NewAlarmDialog dialog(invalidNames, invalidColors, this);

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
  {
    Alarm *alarm = nullptr;
    auto time = QTime(0,0,0);
    auto dateTime = QDateTime::currentDateTime();

    auto alarmWidget = new AlarmWidget();

    initWidget(alarmWidget, dialog);

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

      alarm = new Alarm(seconds, false);
    }

    manageAlarmSignals(alarm);

    alarmWidget->setTime(time);

    m_alarms.insert(alarmWidget, alarm);

    if(!dialog.isTimer())
    {
      alarm->start();
    }
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
void MultiAlarm::onAlarmTic(unsigned long long seconds)
{
  auto alarm = qobject_cast<Alarm *>(sender());
  auto widget = m_alarms.key(alarm);

  QTime time(0,0,0);
  time = time.addSecs(seconds);

  widget->setTime(time);

  // TODO: update dekstop widget.
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmInterval()
{
  auto alarm = qobject_cast<Alarm *>(sender());
  auto widget = m_alarms.key(alarm);


  // TODO: update tray widget of the sender alarm.
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmTimeout()
{
  // TODO: show message dialog and hide desktop/tray if necessary of the sender alarm.
}

//-----------------------------------------------------------------
void MultiAlarm::manageAlarmSignals(Alarm *alarm)
{
  if(alarm->remainingTime() == QTime(0,0,0))
  {
    disconnect(alarm, SIGNAL(tic(unsigned long long)),
               this,  SLOT(onAlarmTic(unsigned long long)));

    disconnect(alarm, SIGNAL(interval()),
               this,  SLOT(onAlarmInterval()));

    disconnect(alarm, SIGNAL(timeout()),
               this,  SLOT(onAlarmTimeout()));
  }
  else
  {
    connect(alarm, SIGNAL(tic(unsigned long long)),
            this,  SLOT(onAlarmTic(unsigned long long)));

    connect(alarm, SIGNAL(interval()),
            this,  SLOT(onAlarmInterval()));

    connect(alarm, SIGNAL(timeout()),
            this,  SLOT(onAlarmTimeout()));
  }
}

//-----------------------------------------------------------------
void MultiAlarm::initWidget(AlarmWidget *widget, const NewAlarmDialog& dialog)
{
  centralWidget()->layout()->addWidget(widget);
  setFixedHeight(size().height() + widget->size().height());

  widget->setColor(dialog.color());
  widget->setName(dialog.name());
  widget->setToolTip(dialog.name() + QString(" Alarm"));

  connect(widget, SIGNAL(startAlarm()),
          this,   SLOT(onAlarmStarted()));

  connect(widget, SIGNAL(stopAlarm()),
          this,   SLOT(onAlarmStopped()));

  connect(widget, SIGNAL(deleteAlarm()),
          this,   SLOT(onAlarmDeleted()));
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmStarted()
{
  auto widget = qobject_cast<AlarmWidget *>(sender());

  m_alarms[widget]->start();
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmStopped()
{
  auto widget = qobject_cast<AlarmWidget *>(sender());

  auto alarm = m_alarms[widget];

  alarm->stop();
  widget->setTime(alarm->remainingTime());
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmDeleted()
{
  auto widget = qobject_cast<AlarmWidget *>(sender());

  centralWidget()->layout()->removeWidget(widget);
  setFixedHeight(size().height() - widget->size().height());

  auto alarm = m_alarms[widget];

  m_alarms.remove(widget);

  delete widget;
  delete alarm;
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
