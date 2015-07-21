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
#include <Alarm.h>
#include "ui_AlarmWidget.h"

// Qt
#include <QWidget>
#include <QSystemTrayIcon>

class QTime;

class AlarmWidget
: public QWidget
, public Ui_AlarmWidget
{
    Q_OBJECT
  public:
    /** \brief AlarmWidget class constructor.
     * \param[in] parent pointer of the QWidget parent of this one.
     * \param[in] flags window flags.
     *
     */
    explicit AlarmWidget(QWidget * parent = nullptr, Qt::WindowFlags flags = 0);

    /** \brief AlarmWidget class virtual destructor.
     *
     */
    virtual ~AlarmWidget();

    /** \brief Sets the name of the alarm.
     * \param[in] name alarm's name.
     *
     */
    void setName(const QString &name);

    /** \brief Returns the name of the alarm.
     *
     */
    const QString name() const;

    /** \brief Sets the color of the alarm.
     * \param[in] colorName color name belonging to QColor::colorNames();
     *
     */
    void setColor(const QString &colorName);

    /** \brief Returns the name of the color of the alarm.
     *
     */
    const QString color() const;

    /** \brief Sets the alarm.
     *
     */
    void setAlarm(Alarm *alarm);

    /** \brief Hides the start/stop button, used only in clock alarms.
     *
     */
    void hideStartButton();

    /** \brief Enables/disables the tray icon indicator.
     * \param[in] value true to enable and false otherwise.
     *
     */
    void useTrayIcon(bool value);

    /** \brief Enables/disables the desktop widget indicator.
     * \param[in] value true to enable and false otherwise.
     *
     */
    void useDesktopWidget(bool value);

    /** \brief Starts the alarm.
     *
     */
    void start();

  signals:
    void deleteAlarm();

  private slots:
    /** \brief Modifies the UI and desktop widget time.
     *
     */
    void onAlarmTic();

    /** \brief Modifies the tray icon.
     * \param[in] value completed intervals.
     *
     */
    void onAlarmInterval(int value);

    /** \brief Shows the timeout dialog.
     *
     */
    void onAlarmTimeout();

    /** \brief Modifies the UI and signals the start of the alarm.
     *
     */
    void onPlayPressed();

    /** \brief Signals the deletion of the alarm.
     *
     */
    void onDeletePressed();

  private:
    /** \brief Sets the duration of the alarm's timer.
     * \param[in] time alarm's timer duration.
     *
     */
    void setTime(const Alarm::AlarmTime& time);

    /** \brief Modifies the color of the tray icon to match the color of the alarm.
     *
     */
    void modifyIconColor();

    bool    m_started;    /** true if the alarm has been started and false otherwise. */
    QString m_color;      /** color of the text of the widget (always black or white) */
    QString m_alarmName;  /** name of the alarm.                                      */
    QString m_alarmColor; /** name of the color of the alarm.                         */
    Alarm  *m_alarm;      /** alarm class object.                                     */

    QSystemTrayIcon *m_icon; /** system tray icon */

};

#endif // ALARMWIDGET_H_
