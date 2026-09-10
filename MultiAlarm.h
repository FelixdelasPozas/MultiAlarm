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
#include <AlarmWidget.h>
#include <NewAlarmDialog.h>
#include <Utils.h>
#include <ui_MainWindow.h>

// Qt
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSettings>
#include <QNetworkAccessManager>

// C++
#include <memory>

class QEvent;
class QCloseEvent;
class QSettings;
class QNetworkReply;

/** \class MultiAlarm
 * \brief Application main window.
 */
class MultiAlarm
: public QMainWindow
, private Ui_MainWindow
{
    Q_OBJECT
  public:
    /** \brief MultiAlarm class constructor.
     */
    MultiAlarm(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    /** \brief MultiAlarm class virtual destructor.
     */
    virtual ~MultiAlarm();

    /** \brief Return the list of used alarm names.
     */
    QStringList usedNames() const;

    /** \brief Returns the list of used colors.
     */
    QStringList usedColors() const;

  private slots:
    /** \brief Launches the alarm creation dialog.
     */
    void createNewAlarm();

    /** \brief Launches the about dialog.
     */
    void aboutDialog();

    /** \brief Launches the settings dialog.
     */
    void settingsDialog();

    /** \brief Handles the icon tray activation.
     * \param[in] reason reason for activation.
     */
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    /** \brief Restores the main dialog and hides the tray icon.
     */
    void onRestoreActionActivated();

    /** \brief Cancels all alarms and exits the application.
     */
    void onQuitActionActivated();

    /** \brief Deletes the alarm and widget.
     */
    void onAlarmDeleted();

    /** \brief Shows the main dialog if configured to do so.
     */
    void onAlarmFinished();
    
    /** \brief Handles network replies.
     * \param[in] reply network reply object pointer.
     */
    void replyFinished(QNetworkReply *reply);

  private:
    virtual void changeEvent(QEvent *e) override;
    virtual void closeEvent(QCloseEvent *e) override;

    /** \brief Creates a widget using the data from the dialog.
     * \param[in] settings settings object containinf the alarm properties.
     * \param[in] name alarm identificator.
     */
    AlarmWidget *createAlarmWidget(QSettings &settings, const QString &name);

    /** \brief Creates a widget using the data from the dialog.
     * \param[in] dialog finished dialog with the alarm properties.
     */
    AlarmWidget *createAlarmWidget(const NewAlarmDialog &dialog);

    /** \brief Restores application settings from ini file.
     */
    void restoreSettings();

    /** \brief Saves application settings to ini file.
     */
    void saveSettings() const;

    /** \brief Helper method to setup the tray icon.
     */
    void setupTrayIcon();

    /** \brief Makes all the connections between QObjects.
     */
    void connectSignals();

    /** \brief Adds an alarm to the list of alarms and configures the signals and UI.
     * \param[in] widget widget to add.
     */
    void addAlarmWidget(AlarmWidget *widget);

    /** \brief Returns the current height of the main window.
     */
    int currentHeight() const;

    /** \brief Returns the application settings. From INI file if exists or the registry if not.
     *         Needs to be a pointer because QSettings is not copyable.
     */
    std::unique_ptr<QSettings> applicationSettings() const;

    /** \brief Helper method to return a tray icon appropiate for the user color scheme in tray.
     */
    QIcon appropiateTrayIcon() const;

    /** \brief Helper method that checks for application updates.
     */
    void checkForUpdates();

    /** \brief Parses Gihub reply data.
     * \param[in] data Github reply data.
     */
    void processGithubData(const QByteArray &data);

  private:
    QSystemTrayIcon *m_icon;              /** application icon when minimized.        */
    QAction         *m_restoreMenuAction; /** restore application tray icon action.   */
    QAction         *m_quitMenuAction;    /** quit application tray icon action.      */
    bool             m_needsExit;         /** true to exit the application on close.  */
    QTimer           m_updatesTimer;      /** timer to check for application updates. */
    Configuration    m_configuration;     /** application configuration data.         */

    QList<AlarmWidget *> m_alarms;        /** alarms widgets.                         */

    std::unique_ptr<QNetworkAccessManager> m_netManager;   /** network manager.       */
};

#endif // MULTIALARM_H_
