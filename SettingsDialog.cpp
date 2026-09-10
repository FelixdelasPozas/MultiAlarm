/*
 File: SettingsDialog.cpp
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

// Project
#include <SettingsDialog.h>
#include <Utils.h>

// Qt
#include <QFile>

//----------------------------------------------------------------------------
SettingsDialog::SettingsDialog(Configuration &config, QWidget* parent, Qt::WindowFlags f)
: QDialog{parent, f}
, m_configuration{config}
{
  setupUi(this);

  m_updatesCheck->setChecked(config.update != Update::NEVER);
  m_updatesCombo->setEnabled(config.update != Update::NEVER);
  m_updatesCombo->setCurrentIndex(std::max(0, static_cast<int>(config.update) - 1));
  m_closeCheck->setChecked(!config.closeIsExit);
  m_showCheck->setChecked(config.raiseOnFinish);

  connectSignals();
}

//----------------------------------------------------------------------------
void SettingsDialog::showEvent(QShowEvent* e)
{
  QDialog::showEvent(e);
  centerDialog(this, 400);
}

//----------------------------------------------------------------------------
void SettingsDialog::connectSignals()
{
  connect(m_updatesCheck, SIGNAL(stateChanged(int)), this, SLOT(onUpdateCheckBoxChanged()));
}

//----------------------------------------------------------------------------
void SettingsDialog::onUpdateCheckBoxChanged()
{
  m_updatesCombo->setEnabled(m_updatesCheck->isChecked());
}

//----------------------------------------------------------------------------
void SettingsDialog::accept()
{
  m_configuration.update = m_updatesCheck->isChecked() ? static_cast<Update>(m_updatesCombo->currentIndex() + 1) : Update::NEVER;
  m_configuration.raiseOnFinish = m_showCheck->isChecked();
  m_configuration.closeIsExit = !m_closeCheck->isChecked();

  QDialog::accept();
}
