/*
 File: AlarmWidget.cpp
 Created on: 10/07/2015
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
#include <Alarm.h>
#include <AlarmWidget.h>
#include <DesktopWidget.h>
#include <MultiAlarm.h>
#include <NewAlarmDialog.h>
#include <LogiLED.h>

// Qt
#include <QTime>
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QSoundEffect>
#include <QTemporaryFile>
#include <QMenu>

const QString COLOR_QSTRING = "<font color='%1'>%2</font>";

const QStringList soundFiles = { ":/MultiAlarm/sounds/Beeper 1.wav",
                                 ":/MultiAlarm/sounds/Beeper 2.wav",
                                 ":/MultiAlarm/sounds/Beeper 3.wav",
                                 ":/MultiAlarm/sounds/Buzzer 1.wav",
                                 ":/MultiAlarm/sounds/Buzzer 2.wav",
                                 ":/MultiAlarm/sounds/Code Red.wav",
                                 ":/MultiAlarm/sounds/Electronic.wav",
                                 ":/MultiAlarm/sounds/Pager.wav",
                                 ":/MultiAlarm/sounds/Smoke.wav" };

//-----------------------------------------------------------------
AlarmWidget::AlarmWidget(MultiAlarm *parent, Qt::WindowFlags flags)
: QWidget        {parent, flags}
, m_started      {false}
, m_contrastColor{"black"}
, m_alarm        {nullptr}
, m_icon         {nullptr}
, m_widget       {nullptr}
, m_logiled      {nullptr}
, m_sound        {nullptr}
, m_soundFile    {nullptr}
, m_parent       {parent}
{
  setupUi(this);

  connect(m_start, SIGNAL(clicked(bool)),
          this,    SLOT(onPlayPressed()));

  connect(m_stop,  SIGNAL(clicked(bool)),
          this,    SLOT(onStopPressed()));

  connect(m_delete, SIGNAL(clicked(bool)),
          this,     SLOT(onDeletePressed()));

  connect(m_settings, SIGNAL(clicked(bool)),
          this, SLOT(onSettingsPressed()));

  m_stop->setEnabled(false);
}

//-----------------------------------------------------------------
AlarmWidget::~AlarmWidget()
{
  delete m_alarm;

  if(m_icon)
  {
    m_icon->hide();
    delete m_icon;
  }

  if(m_widget)
  {
    m_widget->hide();
    delete m_widget;
  }
}

//-----------------------------------------------------------------
void AlarmWidget::start()
{
  if(m_configuration.isTimer)
  {
    m_start->setIcon(QIcon(":/MultiAlarm/pause.svg"));
    m_start->setToolTip(tr("Pause alarm"));

    disconnect(m_start, SIGNAL(clicked(bool)),
               this,    SLOT(onPlayPressed()));

    connect(m_start, SIGNAL(clicked(bool)),
            this,    SLOT(onPausePressed()));

    m_stop->setEnabled(true);
    m_settings->setEnabled(false);
  }
  else
  {
    m_start->hide();
    m_stop->hide();
    m_settings->hide();
  }

  if(m_icon)
  {
    setTrayIcon(":/MultiAlarm/0.ico");
    m_icon->show();
  }

  if(m_widget)
  {
    m_widget->show();
  }

  if(m_logiled)
  {
    m_logiled->registerItem(name(), m_alarm->progress(), QColor{color()}, QColor{m_contrastColor});
  }

  m_alarm->start();
  m_started = true;
}

//-----------------------------------------------------------------
void AlarmWidget::stop()
{
  if(!m_alarm->isRunning())
  {
    onPausePressed();
  }
  m_alarm->stop();
  m_started = false;

  m_stop->setEnabled(false);

  if(m_configuration.isTimer)
  {
    m_start->setIcon(QIcon(":/MultiAlarm/play.svg"));
    m_start->setToolTip(tr("Start alarm"));

    disconnect(m_start, SIGNAL(clicked(bool)),
               this,    SLOT(onPausePressed()));

    connect(m_start, SIGNAL(clicked(bool)),
            this,    SLOT(onPlayPressed()));
  }
  m_settings->setEnabled(true);

  if(m_icon)
  {
    m_icon->hide();
  }

  if(m_widget)
  {
    m_widget->setProgress(0);
    m_widget->hide();
  }

  if(m_logiled)
  {
    m_logiled->unregisterItem(name());
  }

  setTime(m_alarm->remainingTime());
}

//-----------------------------------------------------------------
const QString AlarmWidget::name() const
{
  return m_configuration.name;
}

//-----------------------------------------------------------------
const QString AlarmWidget::color() const
{
  return m_configuration.color;
}

//-----------------------------------------------------------------
void AlarmWidget::setTime(const Alarm::AlarmTime& time)
{
  auto timeString = time.text();

  m_time->setText(COLOR_QSTRING.arg(m_contrastColor).arg(timeString));
}

//-----------------------------------------------------------------
void AlarmWidget::setColor(const QString& colorName)
{
  auto color = QColor(colorName);

  auto blackDistance = color.red() + color.green() + color.blue();
  auto whiteDistance = (254 * 3) - blackDistance;

  m_contrastColor = (blackDistance < whiteDistance ? "white" : "black");
  auto otherColor = (blackDistance > whiteDistance ? "white" : "black");
  auto shineColor = QColor(otherColor);
  auto mix = QColor((color.red()+ 3*shineColor.red())/4, (color.green()+ 3*shineColor.green())/4, (color.blue()+ 3*shineColor.blue())/4);

  m_name->setText(COLOR_QSTRING.arg(m_contrastColor).arg(m_name->text()));
  m_time->setText(COLOR_QSTRING.arg(m_contrastColor).arg(m_time->text()));

  auto style = QString("QFrame#m_frame { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:0.4 %2, stop:0.6 %2, stop:1 %1) }").arg(colorName).arg(mix.name(QColor::NameFormat::HexRgb));
  setStyleSheet(style);
  update();
}

//-----------------------------------------------------------------
void AlarmWidget::onPlayPressed()
{
  if(m_started) return;

  start();
}

//-----------------------------------------------------------------
void AlarmWidget::onPausePressed()
{
  if(m_icon)
  {
    auto action = m_icon->contextMenu()->actions().first();

    if(m_alarm->isRunning())
    {
      action->setIcon(QIcon(":/MultiAlarm/play.svg"));
      action->setText(tr("Resume alarm"));
    }
    else
    {
      action->setIcon(QIcon(":/MultiAlarm/pause.svg"));
      action->setText(tr("Pause alarm"));
    }
  }

  if(m_configuration.isTimer)
  {
    if(m_alarm->isRunning())
    {
      m_start->setIcon(QIcon(":/MultiAlarm/play.svg"));
      m_start->setToolTip(tr("Resume alarm"));
    }
    else
    {
      m_start->setIcon(QIcon(":/MultiAlarm/pause.svg"));
      m_start->setToolTip(tr("Pause alarm"));
    }
  }

  m_alarm->pause(m_alarm->isRunning());
}

//-----------------------------------------------------------------
void AlarmWidget::onStopPressed()
{
  if(!m_started) return;

  stop();
}

//-----------------------------------------------------------------
void AlarmWidget::setAlarm(Alarm* alarm)
{
  m_alarm = alarm;

  setTime(m_alarm->remainingTime());

  connect(alarm, SIGNAL(tic()),
          this,  SLOT(onAlarmTic()), Qt::DirectConnection);

  connect(alarm, SIGNAL(interval(int)),
          this,  SLOT(onAlarmInterval(int)), Qt::DirectConnection);

  connect(alarm, SIGNAL(timeout()),
          this,  SLOT(onAlarmTimeout()), Qt::DirectConnection);
}

//-----------------------------------------------------------------
void AlarmWidget::onAlarmTic()
{
  setTime(m_alarm->remainingTime());

  if(m_icon)
  {
    m_icon->setToolTip(QString("%1\nRemaining time: %2\nCompleted: %3%").arg(m_configuration.name).arg(m_alarm->remainingTimeText()).arg(m_alarm->progress()));
  }

  if(m_widget)
  {
    m_widget->setProgress(m_alarm->precisionProgress());
  }

  if(m_logiled)
  {
    m_logiled->updateItem(name(), m_alarm->progress());
  }
}

//-----------------------------------------------------------------
void AlarmWidget::onAlarmInterval(int value)
{
  if(m_icon)
  {
    setTrayIcon(QString(":/MultiAlarm/%1.ico").arg(value));
  }
}

//-----------------------------------------------------------------
void AlarmWidget::onAlarmTimeout()
{
  if(m_configuration.isTimer && !m_configuration.timerLoops)
  {
    stop();
  }

  auto dialog = new QMessageBox(QMessageBox::Information,
                                m_configuration.name,
                                m_configuration.message,
                                QMessageBox::Ok,
                                this,
                                Qt::Dialog|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowStaysOnTopHint);
  dialog->setWindowIcon(QIcon(":MultiAlarm/2.ico"));

  connect(dialog, SIGNAL(finished(int)),
          this,   SLOT(onDialogFinished()));

  m_sound = new QSoundEffect(this);
  m_soundFile = QTemporaryFile::createNativeFile(soundFiles[m_configuration.sound]);
  m_sound->setSource(QUrl::fromLocalFile(m_soundFile->fileName()));
  m_sound->setLoopCount(QSoundEffect::Infinite);
  m_sound->setVolume(m_configuration.soundVolume/100.0);

  m_sound->play();
  dialog->open();
}

//-----------------------------------------------------------------
void AlarmWidget::onDialogFinished()
{
  m_sound->stop();
  delete m_sound;
  m_sound = nullptr;
  delete m_soundFile;
  m_soundFile = nullptr;

  auto dialog = qobject_cast<QMessageBox *>(sender());
  dialog->deleteLater();

  if(!m_configuration.isTimer)
  {
    emit deleteAlarm();
  }
}

//-----------------------------------------------------------------
void AlarmWidget::onSettingsPressed()
{
  auto names  = m_parent->usedNames();
  names.removeOne(m_configuration.name);
  auto colors = m_parent->usedColors();
  colors.removeOne(m_configuration.color);

  NewAlarmDialog dialog(names, colors, this);
  dialog.setWindowTitle(tr("Modify Alarm"));
  dialog.setName(m_configuration.name);
  dialog.setMessage(m_configuration.message);
  dialog.setColor(m_configuration.color);
  dialog.setIsTimer(m_configuration.isTimer);
  dialog.setTimerTime(m_configuration.timerTime);
  dialog.setClockDateTime(m_configuration.clockDateTime);
  dialog.setTimerLoop(m_configuration.timerLoops);
  dialog.setSound(m_configuration.sound);
  dialog.setSoundVolume(m_configuration.soundVolume);
  dialog.setShowInTray(m_configuration.useTray);
  dialog.setShowInDesktop(m_configuration.useDesktopWidget);
  dialog.setShowInKeyboard(m_configuration.useLogiled);
  dialog.setDesktopWidgetPosition(m_configuration.widgetPosition);
  dialog.setWidgetOpacity(m_configuration.widgetOpacity);

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
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

    setConfiguration(conf);
  }
}

//-----------------------------------------------------------------
void AlarmWidget::onDeletePressed()
{
  emit deleteAlarm();
}

//-----------------------------------------------------------------
void AlarmWidget::setConfiguration(const AlarmConfiguration &conf)
{
  if(m_icon)
  {
    m_icon->hide();
    delete m_icon;
    m_icon = nullptr;
  }

  if(m_widget)
  {
    m_widget->hide();
    delete m_widget;
    m_widget = nullptr;
  }

  if(m_logiled)
  {
    m_logiled = nullptr;
  }

  if(m_alarm)
  {
    delete m_alarm;
    m_alarm = nullptr;
  }

  m_configuration = conf;

  setColor(m_configuration.color);
  m_name->setText(COLOR_QSTRING.arg(m_contrastColor).arg(conf.name));
  setToolTip(conf.name + QString(" Alarm"));

  Alarm *alarm = nullptr;

  if(conf.isTimer)
  {
    Alarm::AlarmTime alarmTime(0, conf.timerTime.hour(), conf.timerTime.minute(), conf.timerTime.second());
    alarm = new Alarm(alarmTime, conf.timerLoops);
  }
  else
  {
    m_start->hide();

    auto now = QDateTime::currentDateTime();
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    constexpr long long int secondsInHour = 60*60;
    constexpr long long int secondsInDay = 24*secondsInHour;

    long long int difference = now.secsTo(conf.clockDateTime);

    float fDays = static_cast<float>(difference)/secondsInDay;
    while(fDays >= 1.f)
    {
      ++days;
      difference -= secondsInDay;
      fDays = static_cast<float>(difference)/secondsInDay;
    }

    float fHours = static_cast<float>(difference)/secondsInHour;
    if(fHours >= 1.f)
    {
      ++hours;
      difference -= secondsInHour;
      fHours = static_cast<float>(difference)/secondsInHour;
    }

    float fMin = static_cast<float>(difference)/60;
    while(fMin >= 1.f)
    {
      ++minutes;
      difference -= 60;
      fMin = static_cast<float>(difference)/60;
    }

    seconds = difference;

    Alarm::AlarmTime alarmTime(days, hours, minutes, seconds);
    alarm = new Alarm(alarmTime, false);
  }

  setAlarm(alarm);

  if(conf.useTray && QSystemTrayIcon::isSystemTrayAvailable())
  {
    m_icon = new QSystemTrayIcon(this);
    m_icon->setToolTip(QString("%1\nRemaining time: %2").arg(m_configuration.name).arg(alarm->remainingTimeText()));

    if(conf.isTimer)
    {
      auto menu = new QMenu();

      auto pauseAlarm = new QAction(QIcon(":/MultiAlarm/pause.svg"), tr("Pause"), this);
      menu->addAction(pauseAlarm);

      connect(pauseAlarm, SIGNAL(triggered()),
              this,    SLOT(onPausePressed()));

      auto stopAlarm = new QAction(QIcon(":/MultiAlarm/stop.svg"), tr("Stop"), this);
      menu->addAction(stopAlarm);

      connect(stopAlarm, SIGNAL(triggered()),
              this,    SLOT(onStopPressed()));

      auto deleteAlarm = new QAction(QIcon(":/MultiAlarm/delete.ico"), tr("Delete"), this);
      menu->addAction(deleteAlarm);

      connect(deleteAlarm, SIGNAL(triggered()),
              this,    SLOT(onDeletePressed()));

      m_icon->setContextMenu(menu);
    }

    setTrayIcon(":/MultiAlarm/0.ico");
  }

  if(conf.useDesktopWidget)
  {
    m_widget = new DesktopWidget(false, nullptr);
    m_widget->setName(m_configuration.name);
    m_widget->setPosition(m_configuration.widgetPosition);
    m_widget->setColor(m_configuration.color);
    m_widget->setOpacity(m_configuration.widgetOpacity);
  }

  if(conf.useLogiled && LogiLED::isAvailable())
  {
    m_logiled = &LogiLED::getInstance();
  }

  if(!conf.isTimer)
  {
    start();
  }
}

//-----------------------------------------------------------------
const struct AlarmConfiguration AlarmWidget::alarmConfiguration() const
{
  return m_configuration;
}

//-----------------------------------------------------------------
void AlarmWidget::setTrayIcon(const QString &icon)
{
  auto qIcon = QIcon(icon);

  auto qPixmap = qIcon.pixmap(128,128);
  auto qimage  = qPixmap.toImage();
  auto blackMask = qPixmap.createMaskFromColor(Qt::black).toImage();
  auto whiteMask = qPixmap.createMaskFromColor(Qt::white).toImage();

  auto color = QColor(m_configuration.color);
  auto colorqrgb = qRgb(color.red(), color.green(), color.blue());
  auto otherColor = QColor(m_contrastColor);
  auto otherColorqrgb = qRgb(otherColor.red(), otherColor.green(), otherColor.blue());

  for(int y = 0; y < blackMask.height(); ++y)
  {
    for(int x = 0; x < blackMask.width(); ++x)
    {
      if(blackMask.pixel(x,y) == 0xffffffff)
      {
        qimage.setPixel(x,y,colorqrgb);
      }
      else
      {
        if(whiteMask.pixel(x,y) == 0xffffffff)
        {
          qimage.setPixel(x,y, otherColorqrgb);
        }
      }
    }
  }

  m_icon->setIcon(QIcon(QPixmap::fromImage(qimage)));
}
