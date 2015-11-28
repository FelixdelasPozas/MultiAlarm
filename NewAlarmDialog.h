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
#include <DesktopWidget.h>

// Qt
#include <QDialog>

class QSoundEffect;
class QTemporaryFile;
class QRect;

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

    /** \brief Sets the type of the alarm.
     * \param[in] value true to set type to timer and false to set to clock.
     *
     */
    void setIsTimer(bool value);

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

    /** \brief Sets the value of the volume.
     * \param[in] value int value in [0-100]
     *
     */
    void setSoundVolume(int value);

    /** \brief Returns the volume of the sound.
     *
     */
    int soundVolume() const;

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

    /** \brief Sets the position of the desktop widget.
     * \param[in] position position coordinates. Must be one in the default positions.
     *
     */
    void setDesktopWidgetPosition(const QPoint &position);

    /** \brief Returns the position of the desktop widget.
     *
     */
    const QPoint desktopWidgetPosition() const;

    /** \brief Sets the opacity for the widget.
     * \param[in] opacity widget opacity in [0-100].
     *
     */
    void setWidgetOpacity(int opacity);

    /** \brief Returns the widget opacity.
     *
     */
    int widgetOpacity() const;

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

    /** \brief Updates the GUI when the desktop widget checkbox changes state.
     * \param[in] value checkbox state.
     *
     */
    void onDesktopWidgetStateChanged(int value);

    /** \brief Updates the opacity slider value when the slider changes.
     * \param[in] value slider position value.
     *
     */
    void onOpacityValueChanged(int value);

    /** \brief Updates the widget position when the combobox changes value.
     * \param[in] value combobox index.
     *
     */
    void onWidgetPositionChanged(int value);

    /** \brief Updates the widget color when the combobox changes value.
     * \param[in] value combobox index.
     *
     */
    void onColorChanged(int value);

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

    /** \brief Updates the GUI when the user repositions the desktop widget.
     *
     */
    void onWidgetBeingDragged();

    /** \brief Updates the GUI when the user changes the sound volume slider.
     * \param[in] value slider value.
     *
     */
    void onVolumeChanged(int value);

  private:
    /** \brief Helper methods to connect the signals for UI elements.
     *
     */
    void connectSignals();

    /** \brief Loads the sounds into the sounds vector.
     *
     */
    void loadSounds();

    /** \brief Fills the desktop widget position combo box with all the
     * posible pre-set positions based on detected screen geometry.
     *
     */
    void computeDesktopWidgetPositions();

    /** \brief Computes all the fixed positions and position names for the given QRect.
     * \param[in] rect QRect rectangle.
     * \param[in] screenName name of the screen.
     * \param[out] positionNames list of position names.
     *
     */
    void computePositions(const QRect &rect, const QString &screenName, QStringList &positionNames);

    QVector<QSoundEffect *> m_sounds;
    QList<QTemporaryFile *> m_temporaryFiles;
    QStringList             m_invalidNames;
    QStringList             m_colors;
    DesktopWidget           m_widget;
    QList<QPoint>           m_widgetPositions;
};

#endif // NEWALARMDIALOG_H_
