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

//-----------------------------------------------------------------
MultiAlarm::MultiAlarm(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow{parent, flags}
, m_icon     {new QSystemTrayIcon(QIcon(":/MultiAlarm/application.ico"), this)}
{
  setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
  setupUi(this);

  statusbar->setVisible(false);
  setFixedHeight(size().height());

  connectSignals();
}

//-----------------------------------------------------------------
MultiAlarm::~MultiAlarm()
{
}

//-----------------------------------------------------------------
void MultiAlarm::createNewAlarm()
{
  NewAlarmDialog dialog(this);

  dialog.exec();
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
    show();
  }
}

//-----------------------------------------------------------------
void MultiAlarm::connectSignals()
{
  connect(m_quitAction, SIGNAL(triggered()),
          this,         SLOT(close()));

  connect(m_newAction, SIGNAL(triggered()),
          this,        SLOT(createNewAlarm()));

  connect(m_newButton, SIGNAL(pressed()),
          this,        SLOT(createNewAlarm()));

  connect(m_aboutAction, SIGNAL(triggered()),
          this,          SLOT(aboutDialog()));

  connect(m_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this,   SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
}
