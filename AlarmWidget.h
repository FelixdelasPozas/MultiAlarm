/*
 File: AlarmWidget.h
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

#ifndef ALARMWIDGET_H_
#define ALARMWIDGET_H_

// Project
#include "ui_AlarmWidget.h"

// Qt
#include <QWidget>

class QTime;

class AlarmWidget
: public QWidget
, public Ui_AlarmWidget
{
    Q_OBJECT
  public:
    /** \brief AlarmWidget class constructor.
     *
     */
    explicit AlarmWidget();

    /** \brief AlarmWidget class virtual destructor.
     *
     */
    virtual ~AlarmWidget();

    /** \brief Sets the name of the alarm.
     * \param[in] name alarm's name.
     *
     */
    void setName(const QString &name);

    /** \brief Sets the duration of the alarm's timer.
     * \param[in] time alarm's timer duration.
     *
     */
    void setTime(const QTime &time);

    /** \brief Sets the color of the alarm.
     * \param[in] colorName color name belonging to QColor::colorNames();
     *
     */
    void setColor(const QString &colorName);

  signals:
    void startAlarm();
    void stopAlarm();
    void deleteAlarm();

  private slots:
    /** \brief Modifies the UI and signals the start of the alarm.
     *
     */
    void onPlayPressed();

    /** \brief Signals the deletion of the alarm.
     *
     */
    void onDeletePressed();

  private:
    bool m_started; /** true if the alarm has been started and false otherwise. */
};

#endif // ALARMWIDGET_H_
