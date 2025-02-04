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
#include <MultiAlarm.h>
#include <AboutDialog.h>
#include <NewAlarmDialog.h>
#include <LogiLED.h>
#include <Alarm.h>

// Qt
#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QDir>

const int MAX_HEIGHT = 800;
const int BAR_WIDTH  = 15;

const QString STATE    = "State";
const QString GEOMETRY = "Geometry";
const QString ALARMS   = "Alarms";

const QString ALARM_NAME            = "Name";
const QString ALARM_MESSAGE         = "Message";
const QString ALARM_COLOR           = "Color";
const QString ALARM_IS_TIMER        = "Timer";
const QString ALARM_TIMER_LOOP      = "Loops";
const QString ALARM_TIMER_TIME      = "TimerTime";
const QString ALARM_CLOCK_DATETIME  = "ClockDateTime";
const QString ALARM_SOUND           = "Sound";
const QString ALARM_SOUND_VOLUME    = "SoundVolume";
const QString ALARM_USE_TRAY        = "UseTray";
const QString ALARM_USE_DESKTOP     = "UseDesktop";
const QString ALARM_USE_LOGILED     = "UseLogiled";
const QString ALARM_WIDGET_POSITION = "DesktopWidgetPosition";
const QString ALARM_WIDGET_OPACITY  = "DesktopWidgetOpacity";

const QString INI_FILENAME = "MultiAlarm.ini";

//-----------------------------------------------------------------
MultiAlarm::MultiAlarm(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow{parent, flags}
, m_icon     {new QSystemTrayIcon(QIcon(":/MultiAlarm/application.ico"), this)}
, m_needsExit{false}
{
  setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

  setupUi(this);

  setFixedHeight(m_newButton->height() + menubar->height());
  centralWidget()->layout()->setContentsMargins(0,0,0,0);
  centralWidget()->layout()->setSpacing(0);

  m_scrollArea->hide();
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_scrollArea->verticalScrollBar()->hide();

  restoreSettings();

  setupTrayIcon();

  LogiLED::getInstance();

  connectSignals();
}

//-----------------------------------------------------------------
MultiAlarm::~MultiAlarm()
{
  saveSettings();
}

//-----------------------------------------------------------------
QStringList MultiAlarm::usedNames() const
{
  QStringList result;
  for(auto alarm: m_alarms)
  {
    result << alarm->name();
  }

  return result;
}

//-----------------------------------------------------------------
QStringList MultiAlarm::usedColors() const
{
  QStringList result;
  for(auto alarm: m_alarms)
  {
    result << alarm->color();
  }

  return result;
}

//-----------------------------------------------------------------
void MultiAlarm::createNewAlarm()
{
  NewAlarmDialog dialog(usedNames(), usedColors(), this);

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
  if(!m_needsExit)
  {
    hide();
    m_icon->show();

    e->accept();
  }
  else
  {
    if(e) QMainWindow::closeEvent(e);
    QApplication::exit(0);
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

  auto restore = new QAction(QIcon(":/MultiAlarm/application.svg"), tr("Restore"), this);
  connect(restore, SIGNAL(triggered()),
          this,    SLOT(onRestoreActionActivated()));

  auto newAlarm = new QAction(QIcon(":/MultiAlarm/add.ico"), tr("New Alarm..."), this);
  connect(newAlarm, SIGNAL(triggered()),
          this,    SLOT(createNewAlarm()));

  auto about = new QAction(QIcon(":/MultiAlarm/information.svg"), tr("About..."), this);
  connect(about, SIGNAL(triggered()),
          this,  SLOT(aboutDialog()));

  auto quit = new QAction(QIcon(":/MultiAlarm/exit.svg"), tr("Quit"), this);
  connect(quit, SIGNAL(triggered()),
          this, SLOT(onQuitActionActivated()));

  menu->addAction(newAlarm);
  menu->addAction(restore);
  menu->addSeparator();
  menu->addAction(about);
  menu->addAction(quit);

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
  m_needsExit = true;
  if (this->isVisible())
    close();
  else
    closeEvent(nullptr);
}

//-----------------------------------------------------------------
void MultiAlarm::addAlarmWidget(AlarmWidget *widget)
{
  if(!widget) return;

  m_scrollArea->show();

  auto oldHeight = currentHeight();
  auto barEnabled = (oldHeight > MAX_HEIGHT);

  connect(widget, SIGNAL(deleteAlarm()),
          this,   SLOT(onAlarmDeleted()));

  m_alarms << widget;
  m_scrollWidget->layout()->addWidget(widget);

  auto height = currentHeight();
  auto needBar = (height > MAX_HEIGHT);

  if(barEnabled != needBar)
  {
    auto bar = m_scrollArea->verticalScrollBar();
    bar->show();

    height = MAX_HEIGHT;
    setFixedWidth(size().width() + BAR_WIDTH);
  }

  setFixedHeight(height);
}

//-----------------------------------------------------------------
void MultiAlarm::restoreSettings()
{
  auto settings = applicationSettings();

  if(settings->contains(STATE))
  {
    auto state = settings->value(STATE).toByteArray();
    restoreState(state);
  }

  if(settings->contains(GEOMETRY))
  {
    auto geometry = settings->value(GEOMETRY).toByteArray();
    restoreGeometry(geometry);
  }

  QStringList expired;

  settings->beginGroup(ALARMS);
  auto alarmIds = settings->childGroups();
  alarmIds.sort();
  for(auto &alarmName : alarmIds)
  {
    auto alarmWidget = createAlarmWidget(*settings, alarmName);

    if(alarmWidget)
    {
      addAlarmWidget(alarmWidget);
    }
    else
    {
      expired << alarmName;
    }
  }
  settings->endGroup();

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

  delete settings;
}

//-----------------------------------------------------------------
void MultiAlarm::saveSettings() const
{
  auto settings = applicationSettings();

  settings->setValue(STATE, saveState());
  settings->setValue(GEOMETRY, saveGeometry());

  settings->beginGroup(ALARMS);

  for(auto alarm: settings->childGroups())
  {
    settings->remove(alarm);
  }

  if(!m_alarms.empty())
  {
    for(auto widget: m_alarms)
    {
      auto conf = widget->alarmConfiguration();

      settings->beginGroup(widget->name());

      settings->setValue(ALARM_MESSAGE, conf.message);
      settings->setValue(ALARM_COLOR, widget->color());
      settings->setValue(ALARM_IS_TIMER, conf.isTimer);

      if(conf.isTimer)
      {
        settings->setValue(ALARM_TIMER_LOOP, conf.timerLoops);
        settings->setValue(ALARM_TIMER_TIME, conf.timerTime);
      }
      else
      {
        settings->setValue(ALARM_CLOCK_DATETIME, conf.clockDateTime);
      }

      settings->setValue(ALARM_SOUND, conf.sound);
      settings->setValue(ALARM_SOUND_VOLUME, conf.soundVolume);
      settings->setValue(ALARM_USE_TRAY, conf.useTray);
      settings->setValue(ALARM_USE_DESKTOP, conf.useDesktopWidget);
      settings->setValue(ALARM_USE_LOGILED, conf.useLogiled);
      settings->setValue(ALARM_WIDGET_POSITION, conf.widgetPosition);
      settings->setValue(ALARM_WIDGET_OPACITY, conf.widgetOpacity);

      settings->endGroup();
    }
  }

  settings->endGroup();
  settings->sync();

  delete settings;
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmDeleted()
{
  AlarmWidget* widget = qobject_cast<AlarmWidget *>(sender());
  if(!widget) return;

  auto oldHeight = currentHeight();
  auto barEnabled = (oldHeight > MAX_HEIGHT);

  m_scrollWidget->layout()->removeWidget(widget);
  m_alarms.removeOne(widget);
  delete widget;

  auto newHeight = currentHeight();
  auto needBar = (newHeight > MAX_HEIGHT);

  if(barEnabled != needBar)
  {
    auto bar = m_scrollArea->verticalScrollBar();
    bar->hide();

    setFixedWidth(size().width() - BAR_WIDTH);
  }

  if(needBar)
  {
    setFixedHeight(MAX_HEIGHT);
  }
  else
  {
    setFixedHeight(newHeight);
  }

  if(m_alarms.empty())
  {
    m_scrollArea->hide();
  }
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
  conf.soundVolume      = dialog.soundVolume();
  conf.useTray          = dialog.showInTray();
  conf.useDesktopWidget = dialog.showInDesktop();
  conf.useLogiled       = dialog.showInKeyboard();
  conf.widgetPosition   = dialog.desktopWidgetPosition();
  conf.widgetOpacity    = dialog.widgetOpacity();

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
  conf.soundVolume      = settings.value(ALARM_SOUND_VOLUME, 100).toInt();
  conf.useTray          = settings.value(ALARM_USE_TRAY, false).toBool();
  conf.useDesktopWidget = settings.value(ALARM_USE_DESKTOP, false).toBool();
  conf.useLogiled       = settings.value(ALARM_USE_LOGILED, false).toBool();
  conf.widgetPosition   = settings.value(ALARM_WIDGET_POSITION, QPoint{0,0}).toPoint();
  conf.widgetOpacity    = settings.value(ALARM_WIDGET_OPACITY, 60).toInt();

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
int MultiAlarm::currentHeight() const
{
  auto alarmSize = m_alarms.size() * m_newButton->size().height();

  return alarmSize + m_newButton->size().height() + menubar->height();
}

//-----------------------------------------------------------------
void MultiAlarm::connectSignals()
{
  connect(m_quitAction, SIGNAL(triggered()),
          QApplication::instance(), SLOT(quit()));

  connect(m_newAction, SIGNAL(triggered()),
          this,        SLOT(createNewAlarm()));

  connect(m_newButton, SIGNAL(clicked(bool)),
          this,        SLOT(createNewAlarm()));

  connect(m_aboutAction, SIGNAL(triggered()),
          this,          SLOT(aboutDialog()));

  connect(m_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this,   SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
}

//-----------------------------------------------------------------
QSettings* MultiAlarm::applicationSettings() const
{
  QDir applicationDir{QCoreApplication::applicationDirPath()};
  if(applicationDir.exists(INI_FILENAME))
  {
    const auto fInfo = QFileInfo(applicationDir.absoluteFilePath(INI_FILENAME));
    if(fInfo.isWritable())
    {
      return new QSettings(INI_FILENAME, QSettings::IniFormat);
    }
  }

  return new QSettings("Felix de las Pozas Alvarez", "MultiAlarm");
}
