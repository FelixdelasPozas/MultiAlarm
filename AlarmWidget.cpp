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

//-----------------------------------------------------------------
AlarmWidget::AlarmWidget()
: m_started{false}
{
  connect(m_start, SIGNAL(toggled(bool)),
          this,    SLOT(onPlayPressed()));

  connect(m_delete, SIGNAL(toggled(bool)),
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
  m_name->setText(name);
}

//-----------------------------------------------------------------
void AlarmWidget::setTime(const QTime& time)
{
  m_time->setText(time.toString());
}

//-----------------------------------------------------------------
void AlarmWidget::setColor(const QString& colorName)
{
  auto color = QColor(colorName);

  auto blackDistance = color.red() + color.green() + color.blue();
  auto whiteDistance = (254 * 3) - blackDistance;

  auto otherColor = (blackDistance < whiteDistance ? "white" : "black");

  auto style = QString("QWidget#AlarmWidget { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:0.5 %2, stop:1 %1) }").arg(colorName).arg(otherColor);
  setStyleSheet(style);
  repaint();
}

//-----------------------------------------------------------------
void AlarmWidget::onPlayPressed()
{
  m_started = !m_started;
  m_delete->setEnabled(!m_started);

  if(m_started)
  {
    m_start->setIcon(QIcon(":/MultiAlarm/play.ico"));
    m_status->setText("Stopped");
    emit stopAlarm();
  }
  else
  {
    m_start->setIcon(QIcon(":/MultiAlarm/stop.ico"));
    m_status->setText("Running");
    emit startAlarm();
  }
}

//-----------------------------------------------------------------
void AlarmWidget::onDeletePressed()
{
  emit deleteAlarm();
}
