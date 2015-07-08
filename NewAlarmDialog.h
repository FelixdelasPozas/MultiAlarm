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
    NewAlarmDialog(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

    /** \brief NewAlarmDialog class virtual destructor.
     *
     */
    virtual ~NewAlarmDialog();

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
};

#endif // NEWALARMDIALOG_H_
