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
#include "AlarmWidget.h"
#include "NewAlarmDialog.h"
#include "ui_MainWindow.h"

// Qt
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSettings>

// C++
#include <memory>

class QEvent;
class QCloseEvent;
class QSettings;

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

    /** \brief Return the list of used alarm names.
     *
     */
    QStringList usedNames() const;

    /** \brief Returns the list of used colors.
     *
     */
    QStringList usedColors() const;

  private slots:
    /** \brief Launches the alarm creation dialog.
     *
     */
    void createNewAlarm();

    /** \brief Launches the about dialog.
     *
     */
    void aboutDialog();

    /** \brief Handles the icon tray activation.
     * \param[in] reason reason for activation.
     *
     */
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    /** \brief Restores the main dialog and hides the tray icon.
     *
     */
    void onRestoreActionActivated();

    /** \brief Cancels all alarms and exits the application.
     *
     */
    void onQuitActionActivated();

    /** \brief Deletes the alarm and widget.
     *
     */
    void onAlarmDeleted();

  private:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *e);

    /** \brief Creates a widget using the data from the dialog.
     * \param[in] settings settings object containinf the alarm properties.
     * \param[in] name alarm identificator.
     *
     */
    AlarmWidget *createAlarmWidget(QSettings &settings, const QString &name);

    /** \brief Creates a widget using the data from the dialog.
     * \param[in] dialog finished dialog with the alarm properties.
     *
     */
    AlarmWidget *createAlarmWidget(const NewAlarmDialog &dialog);

    /** \brief Restores application settings from ini file.
     *
     */
    void restoreSettings();

    /** \brief Saves application settings to ini file.
     *
     */
    void saveSettings() const;

    /** \brief Helper method to setup the tray icon.
     *
     */
    void setupTrayIcon();

    /** \brief Makes all the connections between QObjects.
     *
     */
    void connectSignals();

    /** \brief Adds an alarm to the list of alarms and configures the signals and UI.
     * \param[in] widget widget to add.
     *
     */
    void addAlarmWidget(AlarmWidget *widget);

    /** \brief Returns the current height of the main window.
     *
     */
    int currentHeight() const;

    /** \brief Returns the application settings. From INI file if exists or the registry if not.
     *         Needs to be a pointer because QSettings is not copyable.
     *
     */
    QSettings* applicationSettings() const;

  private:
    QSystemTrayIcon *m_icon;              /** application icon when minimized.      */
    QAction         *m_restoreMenuAction; /** restore application tray icon action. */
    QAction         *m_quitMenuAction;    /** quit application tray icon action.    */

    QList<AlarmWidget *> m_alarms;        /** alarms widgets.                       */
};

#endif // MULTIALARM_H_
