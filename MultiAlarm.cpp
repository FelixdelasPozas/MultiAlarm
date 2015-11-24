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
#include <QMessageBox>
#include <QDebug>

// C++
#include <iostream>

const QString STATE    = "State";
const QString GEOMETRY = "Geometry";
const QString ALARMS   = "Alarms";

const QString ALARM_NAME           = "Name";
const QString ALARM_MESSAGE        = "Message";
const QString ALARM_COLOR          = "Color";
const QString ALARM_IS_TIMER       = "Timer";
const QString ALARM_TIMER_LOOP     = "Loops";
const QString ALARM_TIMER_TIME     = "TimerTime";
const QString ALARM_CLOCK_DATETIME = "ClockDateTime";
const QString ALARM_SOUND          = "Sound";
const QString ALARM_USE_TRAY       = "UseTray";
const QString ALARM_USE_DESKTOP    = "UseDesktop";

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
    auto alarmWidget = createAlarmWidget(dialog);
    addAlarmWidget(alarmWidget);
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
void MultiAlarm::closeEvent(QCloseEvent *e)
{
  // NOTE: need to force the application to exit because QApplication::QuitOnLastWindowClosed is false.
  QCoreApplication::quit();
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
void MultiAlarm::addAlarmWidget(AlarmWidget *widget)
{
  connect(widget, SIGNAL(deleteAlarm()),
          this,   SLOT(onAlarmDeleted()));

  m_alarms << widget;

  centralWidget()->layout()->addWidget(widget);
  setFixedHeight(size().height() + widget->size().height());
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

  QStringList expired;

  settings.beginGroup(ALARMS);
  for(auto alarmName : settings.childGroups())
  {
    auto alarmWidget = createAlarmWidget(settings, alarmName);

    if(alarmWidget)
    {
      addAlarmWidget(alarmWidget);
    }
    else
    {
      expired << alarmName;
    }
  }
  settings.endGroup();

  if(!expired.empty())
  {
    auto message = QString("The following clock alarms will be deleted because they have expired:\n");
    for(auto alarm: expired)
    {
      message += alarm + QString("\n");
    }

    QMessageBox mb;
    mb.setWindowTitle("Expired Clock Alarms");
    mb.setWindowIcon(QIcon(":/MultiAlarm/application.ico"));
    mb.setText(message);
    mb.exec();
  }
}

//-----------------------------------------------------------------
void MultiAlarm::saveSettings()
{
  QSettings settings("MultiAlarm.ini", QSettings::IniFormat);

  settings.setValue(STATE, saveState());
  settings.setValue(GEOMETRY, saveGeometry());

  settings.beginGroup(ALARMS);

  for(auto alarm: settings.childGroups())
  {
    settings.remove(alarm);
  }

  if(!m_alarms.empty())
  {
    for(auto widget: m_alarms)
    {
      auto conf = widget->alarmConfiguration();

      settings.beginGroup(widget->name());

      settings.setValue(ALARM_MESSAGE, conf.message);
      settings.setValue(ALARM_COLOR, widget->color());
      settings.setValue(ALARM_IS_TIMER, conf.isTimer);

      if(conf.isTimer)
      {
        settings.setValue(ALARM_TIMER_LOOP, conf.timerLoops);
        settings.setValue(ALARM_TIMER_TIME, conf.timerTime);
      }
      else
      {
        settings.setValue(ALARM_CLOCK_DATETIME, conf.clockDateTime);
      }

      settings.setValue(ALARM_SOUND, conf.sound);
      settings.setValue(ALARM_USE_TRAY, conf.useTray);
      settings.setValue(ALARM_USE_DESKTOP, conf.useDesktopWidget);

      settings.endGroup();
    }
  }

  settings.endGroup();
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
AlarmWidget* MultiAlarm::createAlarmWidget(const NewAlarmDialog& dialog)
{
  AlarmConfiguration conf;
  conf.name    = dialog.name();
  conf.message = dialog.message();
  conf.color   = dialog.color();
  conf.isTimer = dialog.isTimer();

  if(conf.isTimer)
  {
    conf.timerTime  = dialog.timerTime();
    conf.timerLoops = dialog.timerLoop();
  }
  else
  {
    conf.clockDateTime = dialog.clockDateTime();
  }

  conf.sound            = dialog.sound();
  conf.useTray          = dialog.showInTray();
  conf.useDesktopWidget = dialog.showInDesktop();

  auto widget = new AlarmWidget(this);
  widget->setConfiguration(conf);

  return widget;
}

//-----------------------------------------------------------------
AlarmWidget* MultiAlarm::createAlarmWidget(QSettings &settings, const QString &name)
{
  settings.beginGroup(name);

  AlarmConfiguration conf;
  conf.name    = name;
  conf.message = settings.value(ALARM_MESSAGE, QString()).toString();
  conf.color   = settings.value(ALARM_COLOR, QString("white")).toString();
  conf.isTimer = settings.value(ALARM_IS_TIMER, false).toBool();

  if(conf.isTimer)
  {
    conf.timerTime  = settings.value(ALARM_TIMER_TIME, QTime(0,1,0)).toTime();
    conf.timerLoops = settings.value(ALARM_TIMER_LOOP, false).toBool();
  }
  else
  {
    conf.clockDateTime = settings.value(ALARM_CLOCK_DATETIME, QDateTime()).toDateTime();
  }

  conf.sound            = settings.value(ALARM_SOUND, 0).toInt();
  conf.useTray          = settings.value(ALARM_USE_TRAY, false).toBool();
  conf.useDesktopWidget = settings.value(ALARM_USE_DESKTOP, false).toBool();

  settings.endGroup();

  if(!conf.isTimer && conf.clockDateTime < QDateTime::currentDateTime())
  {
    return nullptr;
  }

  auto widget = new AlarmWidget(this);
  widget->setConfiguration(conf);

  return widget;
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
