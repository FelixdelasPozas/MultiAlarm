/*
 File: NewAlarmDialog.h
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

#ifndef NEWALARMDIALOG_H_
#define NEWALARMDIALOG_H_

// Project
#include "ui_NewAlarmDialog.h"

// Qt
#include <QDialog>

class QSoundEffect;
class QTemporaryFile;

/** \class NewAlarmDialog
 * \brief Dialog for the creation of an alarm.
 *
 */
class NewAlarmDialog
: public QDialog
, private Ui_NewAlarmDialog
{
    Q_OBJECT
  public:
    /** \brief NewAlarmDialog class constructor.
     * \param[in] parent raw pointer of the QWidget parent of this one.
     * \param[in] flags window flags.
     *
     */
    NewAlarmDialog(QStringList invalidNames, QStringList invalidColors, QWidget * parent = nullptr, Qt::WindowFlags f = 0);

    /** \brief NewAlarmDialog class virtual destructor.
     *
     */
    virtual ~NewAlarmDialog();

    /** \brief Sets the name/identificator of the alarm.
     *
     */
    void setName(const QString &name);

    /** \brief Returns the name/identificator of the alarm.
     *
     */
    const QString name() const;

    /** \brief Sets the message to show when the alarm finishes.
     * \param[in] message text message.
     *
     */
    void setMessage(const QString &message);

    /** \brief Returns the message to show when the alarm finishes.
     *
     */
    const QString message() const;

    /** \brief Returns true if the alarm is a timer, otherwise is a clock.
     *
     */
    bool isTimer() const;

    /** \brief Sets the time of the timer alarm.
     * \param[in] time QTime object.
     *
     */
    void setTimerTime(const QTime &time);

    /** \brief Returns the time of the timer alarm.
     *
     */
    const QTime timerTime() const;

    /** \brief Enables/disables the timer looping.
     * \param[in] value true to loop forever and false otherwise.
     *
     */
    void setTimerLoop(bool value);

    /** \brief Returns true if the timer loops.
     *
     */
    bool timerLoop() const;

    /** \brief Sets the date and time of the clock alarm.
     * \param[in] time QDateTime object.
     *
     */
    void setClockDateTime(const QDateTime &time);

    /** \brief Retuns the date and time of the clock alarm.
     *
     */
    const QDateTime clockDateTime() const;

    /** \brief Sets the color for the tray and desktop notifications.
     * \param[in] colorName color name string.
     *
     */
    void setColor(const QString &colorname);

    /** \brief Returns the color for the tray and desktop notifications.
     *
     */
    const QString color() const;

    /** \brief Sets the index of the sound to be played when the alarm finishes.
     * \param[in] soundIndex index of sound combo box.
     *
     */
    void setSound(int soundIndex);

    /** \brief Returns the index of the selected sound for the alarm.
     *
     */
    int sound() const;

    /** \brief Enables/disables the alarm tray icon.
     * \param[in] value true to enable and false otherwise.
     *
     */
    void setShowInTray(bool value);

    /** \brief Returns true if the alarm will show an icon in the system tray while running.
     *
     */
    bool showInTray() const;

    /** \brief Enables/disables the alarm desktop widget.
     * \param[in] value true to enable and false otherwise.
     *
     */
    void setShowInDesktop(bool value);

    /** \brief Returns true if the alarm will show a widget in the desktop while running.
     *
     */
    bool showInDesktop() const;

  private slots:
    /** \brief Updates UI elements when the timer radio button changes state.
     * \param[in] value true if it's checked and false otherwise.
     *
     */
    void onTimerRadioToggled(bool value);

    /** \brief Updates UI elements when the clock radio button changes state.
     * \param[in] value true if it's checked and false otherwise.
     *
     */
    void onClockRadioToggled(bool value);

    /** \brief Updates the accept button state depending on the state of the name and message fields.
     *
     */
    void checkOkButtonRequirements();

    /** \brief Plays the sound specified by the sound combo box index.
     *
     */
    void playSound();

    /** \brief Restores the icon of the play button when a sound finishes playing.
     *
     */
    void setPlayButtonIcon();

  private:
    /** \brief Helper methods to connect the signals for UI elements.
     *
     */
    void connectSignals();

    /** \brief Loads the sounds into the sounds vector.
     *
     */
    void loadSounds();

    QVector<QSoundEffect *> m_sounds;
    QList<QTemporaryFile *> m_temporaryFiles;
    QStringList             m_invalidNames;
    QStringList             m_colors;
};

#endif // NEWALARMDIALOG_H_
