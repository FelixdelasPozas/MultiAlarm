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

const QString COLOR_QSTRING = "<font color='%1'>%2</font>";

//-----------------------------------------------------------------
AlarmWidget::AlarmWidget(QWidget * parent, Qt::WindowFlags flags)
: QWidget           {parent, flags}
, m_started         {false}
, m_contrastColor   {"black"}
, m_alarm           {nullptr}
, m_icon            {nullptr}
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
}

//-----------------------------------------------------------------
void AlarmWidget::start()
{
  m_start->setIcon(QIcon(":/MultiAlarm/stop.svg"));

  if(m_icon)
  {
    setTrayIcon(":/MultiAlarm/0.ico");
    m_icon->show();
  }

  m_alarm->start();
}

//-----------------------------------------------------------------
void AlarmWidget::stop()
{
  m_alarm->stop();

  m_start->setIcon(QIcon(":/MultiAlarm/play.svg"));

  if(m_icon)
  {
    m_icon->hide();
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

////-----------------------------------------------------------------
//void AlarmWidget::useTrayIcon(bool value)
//{
//  if(value && QSystemTrayIcon::isSystemTrayAvailable() && !m_icon)
//  {
//    m_icon = new QSystemTrayIcon(this);
//
//    // TODO: icon menu
//
//    setTrayIcon(":/MultiAlarm/0.ico");
//  }
//  else
//  {
//    if (m_icon)
//    {
//      delete m_icon;
//    }
//  }
//}

//-----------------------------------------------------------------
void AlarmWidget::setTime(const Alarm::AlarmTime& time)
{
  QString timeString;
  if(time.days != 0)
  {
    timeString += QString("%1 Day").arg(time.days);
    if(time.days > 1)
    {
      timeString += "s";
    }
  }
  timeString += " ";

  for(auto part: {time.hours, time.minutes, time.seconds})
  {
    if(part < 10)
    {
      timeString += "0";
    }
    timeString += QString("%1:").arg(part);
  }
  timeString.remove(timeString.length()-1, 1);

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
  // TODO: show timeout dialog.
}

//-----------------------------------------------------------------
void AlarmWidget::onDeletePressed()
{
  emit deleteAlarm();
}

//-----------------------------------------------------------------
void AlarmWidget::setConfiguration(const AlarmConfiguration conf)
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
    constexpr long int secondsInDay = 24*60*60;
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    days = now.daysTo(conf.clockDateTime);
    if(days > 1)
    {
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

    // TODO: icon menu

    setTrayIcon(":/MultiAlarm/0.ico");
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
