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
#include <AlarmWidget.h>
#include <Alarm.h>

// Qt
#include <QTime>
#include <QDebug>
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QSoundEffect>
#include <QTemporaryFile>
#include <QMenu>

const QString COLOR_QSTRING = "<font color='%1'>%2</font>";

const QStringList soundNames = { ":/MultiAlarm/sounds/buzz.wav",
                                 ":/MultiAlarm/sounds/smokealarm.wav",
                                 ":/MultiAlarm/sounds/deskbell.wav" };

//-----------------------------------------------------------------
AlarmWidget::AlarmWidget(QWidget * parent, Qt::WindowFlags flags)
: QWidget        {parent, flags}
, m_started      {false}
, m_contrastColor{"black"}
, m_alarm        {nullptr}
, m_icon         {nullptr}
, m_sound        {nullptr}
{
  setupUi(this);

  connect(m_start, SIGNAL(clicked(bool)),
          this,    SLOT(onPlayPressed()));

  connect(m_delete, SIGNAL(clicked(bool)),
          this,     SLOT(onDeletePressed()));
}

//-----------------------------------------------------------------
AlarmWidget::~AlarmWidget()
{
  delete m_alarm;

  if(m_icon)
  {
    delete m_icon;
  }

//  if(m_desktopWidget)
//  {
//    delete m_desktopWidget;
//  }
}

//-----------------------------------------------------------------
void AlarmWidget::start()
{
  m_start->setIcon(QIcon(":/MultiAlarm/stop.svg"));
  m_start->setToolTip(tr("Stop alarm"));

  if(m_icon)
  {
    setTrayIcon(":/MultiAlarm/0.ico");
    m_icon->show();
  }

//  if(m_desktopWidget)
//  {
//    m_desktopWidget->show();
//  }

  m_alarm->start();
}

//-----------------------------------------------------------------
void AlarmWidget::stop()
{
  m_alarm->stop();

  m_start->setIcon(QIcon(":/MultiAlarm/play.svg"));
  m_start->setToolTip(tr("Start alarm"));

  if(m_icon)
  {
    m_icon->hide();
  }

//  if(m_desktopWidget)
//  {
//    m_desktopWidget->hide();
//  }

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
  m_started = !m_started;

  m_delete->setEnabled(!m_started);

  if(m_started)
  {
    m_delete->setIcon(QIcon(":/MultiAlarm/delete-disabled.ico"));
    start();
  }
  else
  {
    m_delete->setIcon(QIcon(":/MultiAlarm/delete.ico"));
    stop();
  }
}

//-----------------------------------------------------------------
void AlarmWidget::setAlarm(Alarm* alarm)
{
  m_alarm = alarm;

  setTime(m_alarm->remainingTime());

  connect(alarm, SIGNAL(tic()),
          this,  SLOT(onAlarmTic()));

  connect(alarm, SIGNAL(interval(int)),
          this,  SLOT(onAlarmInterval(int)));

  connect(alarm, SIGNAL(timeout()),
          this,  SLOT(onAlarmTimeout()));
}

//-----------------------------------------------------------------
void AlarmWidget::onAlarmTic()
{
  setTime(m_alarm->remainingTime());

  if(m_icon)
  {
    m_icon->setToolTip(QString("%1\nRemaining time: %2\nCompleted: %3%").arg(m_configuration.name).arg(m_alarm->remainingTimeText()).arg(m_alarm->progress()));
  }

  // TODO: update desktop widget.
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
    onPlayPressed();
  }

  auto dialog = new QMessageBox(this);
  dialog->setWindowIcon(QIcon(":MultiAlarm/2.ico"));
  dialog->setWindowTitle(m_configuration.name);
  dialog->setText(m_configuration.message);
  dialog->setStandardButtons(QMessageBox::Ok);
  dialog->setIcon(QMessageBox::Information);

  connect(dialog, SIGNAL(finished(int)),
          this,   SLOT(onDialogFinished()));

  Q_ASSERT(m_sound == nullptr);

  m_sound = new QSoundEffect(this);
  auto file = QTemporaryFile::createLocalFile(soundNames[m_configuration.sound]);
  m_sound->setSource(QUrl::fromLocalFile(file->fileName()));
  m_sound->setLoopCount(QSoundEffect::Infinite);

  m_sound->play();
  dialog->open();
}

//-----------------------------------------------------------------
void AlarmWidget::onDialogFinished()
{
  m_sound->stop();
  delete m_sound;
  m_sound = nullptr;

  auto dialog = qobject_cast<QMessageBox *>(sender());
  dialog->deleteLater();

  if(!m_configuration.isTimer)
  {
    emit deleteAlarm();
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

    days = now.daysTo(conf.clockDateTime);
    if(days > 1)
    {
      constexpr long int secondsInDay = 24*60*60;
      now = now.addDays(days - 1);

      if(now.secsTo(conf.clockDateTime) > secondsInDay)
      {
        ++days;
        now = now.addSecs(secondsInDay);
      }
    }

    auto remaining = now.secsTo(conf.clockDateTime);
    hours = remaining / 3600;
    remaining -= hours * 3600;
    minutes = remaining / 60;
    remaining -= minutes * 60;
    seconds = remaining;

    Alarm::AlarmTime alarmTime(days, hours, minutes, seconds);
    alarm = new Alarm(alarmTime, false);
  }

  setAlarm(alarm);

  if (conf.useTray && QSystemTrayIcon::isSystemTrayAvailable() && !m_icon)
  {
    m_icon = new QSystemTrayIcon(this);
    m_icon->setToolTip(QString("%1\nRemaining time: %2").arg(m_configuration.name).arg(alarm->remainingTimeText()));

    auto menu = new QMenu();
    auto stopAlarm = new QAction(QIcon(":/MultiAlarm/stop.svg"), tr("Stop alarm"), this);

    menu->addAction(stopAlarm);

    connect(stopAlarm, SIGNAL(triggered()),
            this,    SLOT(onPlayPressed()));

    m_icon->setContextMenu(menu);

    setTrayIcon(":/MultiAlarm/0.ico");
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
