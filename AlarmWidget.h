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
class QSoundEffect;

struct AlarmConfiguration
{
    QString   name;             /** name of the alarm.                                         */
    QString   message;          /** message of the alarm to show on completion.                */
    QString   color;            /** name of the color of the alarm.                            */
    bool      isTimer;          /** true if the alarm is a timer.                              */
    QTime     timerTime;        /** duration of the timer alarm.                               */
    bool      timerLoops;       /** true if the timer loops.                                   */
    QDateTime clockDateTime;    /** date and timer of the clock alarm.                         */
    int       sound;            /** sound used for the finished alarm notification.            */
    bool      useTray;          /** true if the alarm uses the tray for notifications.         */
    bool      useDesktopWidget; /** true if the alarm uses a desktop widget for notifications. */
};

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

    /** \brief Configures the widget.
     * \param[in] configuration AlarmConfiguration containing the details of the alarm.
     *
     */
    void setConfiguration(const AlarmConfiguration conf);

    /** \brief Returns the configuration of the alarm.
     *
     */
    const struct AlarmConfiguration alarmConfiguration() const;

    /** \brief Returns the name of the alarm, needed to avoid duplicated names.
     *
     */
    const QString name() const;

    /** \brief Returns the color of the alarm, needed to avoid duplicated colors.
     *
     */
    const QString color() const;

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

    /** \brief Deletes the dialog and stops the sound if needed.
     *
     */
    void onDialogFinished();

  private:
    /** \brief Starts the alarm and updates the UI.
     *
     */
    void start();

    /** \brief Stops the alarm and updates the UI.
     *
     */
    void stop();

    /** \brief Sets the color of the alarm.
     * \param[in] colorName color name belonging to QColor::colorNames();
     *
     */
    void setColor(const QString &colorName);

    /** \brief Sets the alarm.
     *
     */
    void setAlarm(Alarm *alarm);

    /** \brief Sets the duration of the alarm's timer.
     * \param[in] time alarm's timer duration.
     *
     */
    void setTime(const Alarm::AlarmTime& time);

    /** \brief Modifies the color of the tray icon to match the color of the alarm and sets it.
     *
     */
    void setTrayIcon(const QString &icon);

    bool      m_started;                /** true if the alarm has been started and false otherwise.  */
    QString   m_contrastColor;          /** color of the text of the widget (always black or white). */
    Alarm    *m_alarm;                  /** alarm class object.                                      */
    AlarmConfiguration m_configuration; /** alarm configuration.                                     */
    QSystemTrayIcon   *m_icon;          /** system tray icon.                                        */
    QSoundEffect      *m_sound;         /** alarm sound.                                             */
};

#endif // ALARMWIDGET_H_
