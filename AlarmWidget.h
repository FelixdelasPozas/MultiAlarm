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
class QTemporaryFile;

class DesktopWidget;
class MultiAlarm;
class LogiLED;

/** \struct AlarmConfiguration
 * \brief Holds all the relevant information about an alarm and it's notifications.
 */
struct AlarmConfiguration
{
    QString   name;             /** name of the alarm.                                            */
    QString   message;          /** message of the alarm to show on completion.                   */
    QString   color;            /** name of the color of the alarm.                               */
    bool      isTimer;          /** true if the alarm is a timer.                                 */
    QTime     timerTime;        /** duration of the timer alarm.                                  */
    bool      timerLoops;       /** true if the timer loops.                                      */
    QDateTime clockDateTime;    /** date and timer of the clock alarm.                            */
    int       sound;            /** sound used for the finished alarm notification.               */
    int       soundVolume;      /** volume for the sound in [0 (silence) - 100 (loudest) ]        */
    bool      useTray;          /** true if the alarm uses the tray for notifications.            */
    bool      useDesktopWidget; /** true if the alarm uses a desktop widget for notifications.    */
    bool      useLogiled;       /** true if the alarm uses the keyboard lights for notifications. */
    QPoint    widgetPosition;   /** position of the desktop widget.                               */
    int       widgetOpacity;    /** opacity of the desktop widget in [0-100]                      */
};

/** \class AlarmWidget
 * \brief Widget with alarm information and controls to
 *        start, pause, stop and delete an alarm.
 */
class AlarmWidget
: public QWidget
, public Ui_AlarmWidget
{
    Q_OBJECT
  public:
    /** \brief AlarmWidget class constructor.
     * \param[in] parent Main dialog pointer.
     * \param[in] flags window flags.
     *
     */
    explicit AlarmWidget(MultiAlarm *parent, Qt::WindowFlags flags = Qt::WindowFlags());

    /** \brief AlarmWidget class virtual destructor.
     *
     */
    virtual ~AlarmWidget();

    /** \brief Configures the widget.
     * \param[in] configuration AlarmConfiguration containing the details of the alarm.
     *
     */
    void setConfiguration(const AlarmConfiguration &conf);

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

    /** \brief Modifies the UI and pauses the alarm.
     *
     */
    void onPausePressed();

    /** \brief Modifies the UI and stops the alarm.
     *
     */
    void onStopPressed();

    /** \brief Signals the deletion of the alarm.
     *
     */
    void onDeletePressed();

    /** \brief Deletes the dialog and stops the sound if needed.
     *
     */
    void onDialogFinished();

    /** \brief Opens the configuration dialog.
     *
     */
    void onSettingsPressed();

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
    DesktopWidget     *m_widget;        /** desktop widget.                                          */
    LogiLED           *m_logiled;       /** LogiLED instance.                                        */
    QSoundEffect      *m_sound;         /** alarm sound.                                             */
    QTemporaryFile    *m_soundFile;     /** local sound file in termporary directory.                */

    MultiAlarm *m_parent;               /** main application dialog pointer, used to get the used
                                            names and colors when reconfiguring.                     */
};

#endif // ALARMWIDGET_H_
