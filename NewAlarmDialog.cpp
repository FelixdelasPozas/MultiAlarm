/*
 File: NewAlarmDialog.cpp
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
#include <NewAlarmDialog.h>

// Qt
#include <QImage>
#include <QPainter>
#include <QIcon>
#include <QPixmap>

const QStringList sounds{ "Buzz", "Smoke alarm", "Desk bell"};

//-----------------------------------------------------------------
NewAlarmDialog::NewAlarmDialog(QWidget * parent, Qt::WindowFlags flags)
: QDialog(parent)
{
  setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowContextHelpButtonHint);
  setupUi(this);

  auto dateTime = QDateTime::currentDateTime();
  m_clock->setDate(dateTime.date());
  m_clock->setTime(dateTime.time());

  m_timerRadio->setChecked(true);
  m_timerRadio->setAutoExclusive(true);

  auto colors = QColor::colorNames();
  for(auto color: colors)
  {
    auto pixmap = new QImage(32, 32, QImage::Format_RGB32);
    QPainter painter(pixmap);
    painter.fillRect(0,0,31,31, color);
    painter.end();
    m_colorComboBox->insertItem(colors.indexOf(color), QIcon(QPixmap::fromImage(*pixmap)), color);
  }
  m_colorComboBox->setCurrentIndex(0);

  m_soundComboBox->insertItems(0, sounds);
  m_soundComboBox->setCurrentIndex(0);
}

//-----------------------------------------------------------------
NewAlarmDialog::~NewAlarmDialog()
{
}

