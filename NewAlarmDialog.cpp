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

#include <NewAlarmDialog.h>

//-----------------------------------------------------------------
NewAlarmDialog::NewAlarmDialog(QWidget * parent, Qt::WindowFlags flags)
: QDialog(parent)
{
  setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowContextHelpButtonHint);
  setupUi(this);
}

//-----------------------------------------------------------------
NewAlarmDialog::~NewAlarmDialog()
{
}

