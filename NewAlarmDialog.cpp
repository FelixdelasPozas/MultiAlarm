/*
 File: NewAlarmDialog.cpp
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

// Project
#include <NewAlarmDialog.h>

// Qt
#include <QImage>
#include <QPainter>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QTemporaryFile>
#include <QSoundEffect>
#include <QDebug>

const QStringList sounds{ "Buzz", "Smoke alarm", "Desk bell"};

//-----------------------------------------------------------------
NewAlarmDialog::NewAlarmDialog(QWidget * parent, Qt::WindowFlags flags)
: QDialog(parent)
{
  setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowContextHelpButtonHint);
  setupUi(this);

  auto dateTime = QDateTime::currentDateTime();
  m_clock->setDate(dateTime.date());
  m_clock->setTime(dateTime.time());
  m_clock->setEnabled(false);

  m_timerRadio->setChecked(true);
  m_timerRadio->setAutoExclusive(true);

  auto colors = QColor::colorNames();
  for(auto color: colors)
  {
    auto pixmap = new QImage(32, 32, QImage::Format_RGB32);
    QPainter painter(pixmap);
    painter.fillRect(0,0,31,31, color);
    painter.end();
    m_colorComboBox->insertItem(colors.indexOf(color), QIcon(QPixmap::fromImage(*pixmap)), color);
  }
  m_colorComboBox->setCurrentIndex(0);

  m_soundComboBox->insertItems(0, sounds);
  m_soundComboBox->setCurrentIndex(0);

  connectSignals();

  loadSounds();

  m_buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
}

//-----------------------------------------------------------------
NewAlarmDialog::~NewAlarmDialog()
{
  for(auto sound: m_sounds)
  {
    delete sound;
  }
  m_sounds.clear();

  for(auto file: m_temporaryFiles)
  {
    delete file;
  }
  m_temporaryFiles.clear();
}

//-----------------------------------------------------------------
void NewAlarmDialog::onTimerRadioToggled(bool value)
{
  m_timerLoop->setEnabled(value);
  m_timer->setEnabled(value);
}

//-----------------------------------------------------------------
void NewAlarmDialog::onClockRadioToggled(bool value)
{
  m_clock->setEnabled(value);
}

//-----------------------------------------------------------------
void NewAlarmDialog::checkOkButtonRequirements()
{
  auto enable = !m_name->text().isEmpty() && !m_message->text().isEmpty();
  m_buttons->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

//-----------------------------------------------------------------
void NewAlarmDialog::playSound()
{
  for(auto i: {0,1,2})
  {
    if(m_sounds[i]->isPlaying())
    {
      m_sounds[i]->stop();
      return;
    }
  }

  m_sounds[m_soundComboBox->currentIndex()]->play();
}

//-----------------------------------------------------------------
void NewAlarmDialog::connectSignals()
{
  connect(m_timerRadio, SIGNAL(toggled(bool)),
          this,         SLOT(onTimerRadioToggled(bool)));

  connect(m_clockRadio, SIGNAL(toggled(bool)),
          this,         SLOT(onClockRadioToggled(bool)));

  connect(m_name, SIGNAL(textChanged(const QString &)),
          this,   SLOT(checkOkButtonRequirements()));

  connect(m_message, SIGNAL(textChanged(const QString &)),
          this,      SLOT(checkOkButtonRequirements()));

  connect(m_playSoundButton, SIGNAL(pressed()),
          this,              SLOT(playSound()));
}

//-----------------------------------------------------------------
void NewAlarmDialog::setPlayButtonIcon()
{
  auto sound = qobject_cast<QSoundEffect *>(sender());
  if(sound->isPlaying())
  {
    m_playSoundButton->setIcon(QIcon(":/MultiAlarm/stop.ico"));
  }
  else
  {
    m_playSoundButton->setIcon(QIcon(":/MultiAlarm/play.ico"));
  }
}

//-----------------------------------------------------------------
void NewAlarmDialog::loadSounds()
{
  // NOTE: Load sound files. QSound can't play a file from the qt resource file
  // so we will dump them first to the temporal directory, then load the resources
  // and delete them.
  auto buzz = QTemporaryFile::createLocalFile(":/MultiAlarm/sounds/buzz.wav");
  m_sounds.insert(0, new QSoundEffect(this));
  m_sounds[0]->setSource(QUrl::fromLocalFile(buzz->fileName()));

  auto smokeAlarm = QTemporaryFile::createLocalFile(":/MultiAlarm/sounds/smokealarm.wav");
  m_sounds.insert(1, new QSoundEffect(this));
  m_sounds[1]->setSource(QUrl::fromLocalFile(smokeAlarm->fileName()));

  auto deskBell = QTemporaryFile::createLocalFile(":/MultiAlarm/sounds/deskbell.wav");
  m_sounds.insert(2, new QSoundEffect(this));
  m_sounds[2]->setSource(QUrl::fromLocalFile(deskBell->fileName()));

  m_temporaryFiles << buzz << smokeAlarm << deskBell;

  for(auto i: {0,1,2})
  {
    connect(m_sounds[i], SIGNAL(playingChanged()),
            this,        SLOT(setPlayButtonIcon()));
  }
}
