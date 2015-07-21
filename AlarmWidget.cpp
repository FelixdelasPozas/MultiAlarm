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
: QWidget  {parent, flags}
, m_started{false}
, m_color  {"black"}
, m_alarm  {nullptr}
, m_icon   {nullptr}
{
  setupUi(this);

  connect(m_start, SIGNAL(clicked(bool)),
          this,    SLOT(onPlayPressed()));

  connect(m_delete, SIGNAL(clicked(bool)),
          this,     SLOT(onDeletePressed()));

  m_status->setText(COLOR_QSTRING.arg(m_color).arg("Stopped"));
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
void AlarmWidget::setName(const QString& name)
{
  m_alarmName = name;
  m_name->setText(COLOR_QSTRING.arg(m_color).arg(name));
}

//-----------------------------------------------------------------
const QString AlarmWidget::name() const
{
  return m_alarmName;
}

//-----------------------------------------------------------------
void AlarmWidget::useTrayIcon(bool value)
{
  if(value && QSystemTrayIcon::isSystemTrayAvailable() && m_icon == nullptr)
  {
    m_icon = new QSystemTrayIcon(QIcon(":/MultiAlarm/0.ico"), this);
    modifyIconColor();
    m_icon->show();
  }
  else
  {
    if (m_icon)
    {
      m_icon->hide();
      delete m_icon;
    }
  }
}

//-----------------------------------------------------------------
void AlarmWidget::useDesktopWidget(bool value)
{
  // TODO
}

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

  m_time->setText(COLOR_QSTRING.arg(m_color).arg(timeString));
}

//-----------------------------------------------------------------
void AlarmWidget::setColor(const QString& colorName)
{
  m_alarmColor = colorName;
  auto color = QColor(colorName);

  auto blackDistance = color.red() + color.green() + color.blue();
  auto whiteDistance = (254 * 3) - blackDistance;

  m_color = (blackDistance < whiteDistance ? "white" : "black");
  auto otherColor = (blackDistance > whiteDistance ? "white" : "black");
  auto oColor = QColor(otherColor);
  auto mix = QColor((color.red()+ 3*oColor.red())/4, (color.green()+ 3*oColor.green())/4, (color.blue()+ 3*oColor.blue())/4);

  m_name->setText(COLOR_QSTRING.arg(m_color).arg(m_name->text()));
  m_time->setText(COLOR_QSTRING.arg(m_color).arg(m_time->text()));
  m_status->setText(COLOR_QSTRING.arg(m_color).arg(m_status->text()));

  auto style = QString("QFrame#m_frame { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:0.5 %2, stop:1 %1) }").arg(colorName).arg(mix.name(QColor::NameFormat::HexRgb));
  setStyleSheet(style);
  update();
}

//-----------------------------------------------------------------
const QString AlarmWidget::color() const
{
  return m_alarmColor;
}

//-----------------------------------------------------------------
void AlarmWidget::onPlayPressed()
{
  m_started = !m_started;

  m_delete->setEnabled(!m_started);

  if(m_started)
  {
    m_start->setIcon(QIcon(":/MultiAlarm/stop.svg"));
    m_status->setText(COLOR_QSTRING.arg(m_color).arg("Running"));
    m_alarm->start();
  }
  else
  {
    m_start->setIcon(QIcon(":/MultiAlarm/play.svg"));
    m_status->setText(COLOR_QSTRING.arg(m_color).arg("Stopped"));
    m_alarm->stop();

    setTime(m_alarm->remainingTime());
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
void AlarmWidget::hideStartButton()
{
  m_start->hide();
  m_status->setText(COLOR_QSTRING.arg(m_color).arg("Running"));
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
  if(!m_icon) return;

  auto icon = QIcon(QString(":/MultiAlarm/%1.ico").arg(value));
  m_icon->setIcon(icon);
  modifyIconColor();
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
void AlarmWidget::modifyIconColor()
{
  auto qpixmap = m_icon->icon().pixmap(128,128);
  auto qimage  = qpixmap.toImage();
  auto mask = qpixmap.createMaskFromColor(Qt::black).toImage();
  auto color = QColor(m_alarmColor);
  auto qrgb = qRgb(color.red(), color.green(), color.blue());

  for(int y = 0; y < mask.height(); ++y)
  {
    for(int x = 0; x < mask.width(); ++x)
    {
      if(mask.pixel(x,y) == 0xffffffff)
      {
        qimage.setPixel(x,y,qrgb);
      }
    }
  }

  m_icon->setIcon(QIcon(QPixmap::fromImage(qimage)));
}
