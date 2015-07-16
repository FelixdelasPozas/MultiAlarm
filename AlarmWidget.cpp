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

// Qt
#include <QTime>
#include <QDebug>

const QString COLOR_QSTRING = "<font color='%1'>%2</font>";

//-----------------------------------------------------------------
AlarmWidget::AlarmWidget(QWidget * parent, Qt::WindowFlags flags)
: QWidget  {parent, flags}
, m_started{false}
, m_color  {"black"}
{
  setupUi(this);

  connect(m_start, SIGNAL(clicked(bool)),
          this,    SLOT(onPlayPressed()));

  connect(m_delete, SIGNAL(clicked(bool)),
          this,     SLOT(onDeletePressed()));

  m_status->setText("Stopped");
}

//-----------------------------------------------------------------
AlarmWidget::~AlarmWidget()
{
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
void AlarmWidget::setTime(const QTime& time)
{
  m_time->setText(COLOR_QSTRING.arg(m_color).arg(time.toString()));
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
  auto mix = QColor((color.red()+ 2*oColor.red())/3, (color.green()+ 2*oColor.green())/3, (color.blue()+ 2*oColor.blue())/3);

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
  }
  else
  {
    m_start->setIcon(QIcon(":/MultiAlarm/play.svg"));
    m_status->setText(COLOR_QSTRING.arg(m_color).arg("Stopped"));
  }
}

//-----------------------------------------------------------------
void AlarmWidget::setAlarm(Alarm* alarm)
{
  m_alarm = alarm;

  connect(alarm, SIGNAL(tic(unsigned long long)),
          this,  SLOT(onAlarmTic(unsigned long long)));

  connect(alarm, SIGNAL(interval()),
          this,  SLOT(onAlarmInterval()));

  connect(alarm, SIGNAL(timeout()),
          this,  SLOT(onAlarmTimeout()));
}

//-----------------------------------------------------------------
void AlarmWidget::onAlarmTic(unsigned long long seconds)
{
  // TODO: update ui and desktop widget.
}

//-----------------------------------------------------------------
void AlarmWidget::onAlarmInterval()
{
  // TODO: update tray icon.
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
