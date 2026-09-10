/*
 File: SettingsDialog.h
 Created on: 09/09/2026
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

#ifndef _SETTINGS_DIALOG_H_
#define _SETTINGS_DIALOG_H_

// Qt
#include <QDialog>

// Project
#include <Utils.h>
#include "ui_SettingsDialog.h"

/** \class SettingsDialog
 * \brief Implements the application settings dialog. 
 */
class SettingsDialog : public QDialog, private Ui::SettingsDialog
{
    Q_OBJECT
  public:
    /** \brief SettingsDialog class constructor. 
     * \param[in] config Application configuration structure. 
     * \param[in] parent Raw pointer of the widget parent of this one. 
     * \param[in] f window flags.
     */
    SettingsDialog(Configuration &config, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    /** \brief SettingsDialog class virtual destructor. 
     */
    virtual ~SettingsDialog() = default;

public slots:
    // Overriding the accept() slot
    void accept() override;    

  protected:
    virtual void showEvent(QShowEvent* e) override;

  private slots:
    /** \brief Updates the UI when the updates checkbox changes state. 
     */
    void onUpdateCheckBoxChanged();

  private:
    /** \brief Helper method to connect signals to slots. 
     */
    void connectSignals();

    Configuration &m_configuration; /** application configuration. */
};

#endif