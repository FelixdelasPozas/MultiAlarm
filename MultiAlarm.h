/*
 File: MultiAlarm.h
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

#ifndef MULTIALARM_H_
#define MULTIALARM_H_

// Project
#include "NewAlarmDialog.h"
#include "ui_MainWindow.h"

// Qt
#include <QMainWindow>

/** \class MultiAlarm
 * \brief Application main window.
 *
 */
class MultiAlarm
: public QMainWindow
, private Ui_MainWindow
{
    Q_OBJECT
  public:
    /** \brief MultiAlarm class constructor.
     *
     */
    MultiAlarm(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    /** \brief MultiAlarm class virtual destructor.
     *
     */
    virtual ~MultiAlarm();

  private slots:
    /** \brief Launches the alarm creation dialog.
     *
     */
    void createNewAlarm();

    /** \brief Manages the creation of a new alarm.
     * \param[in] dialog recently closed new alarm dialog pointer.
     *
     */
    void onAlarmDialogClosed(NewAlarmDialog *dialog);

  private:
    /** \brief Makes all the connections between QObjects.
     *
     */
    void connectSignals();
};

#endif // MULTIALARM_H_
