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
#include <QSystemTrayIcon>

const QStringList sounds{ "Buzz", "Smoke alarm", "Desk bell"};

const QStringList soundNames = { ":/MultiAlarm/sounds/buzz.wav",
                                 ":/MultiAlarm/sounds/smokealarm.wav",
                                 ":/MultiAlarm/sounds/deskbell.wav" };

//-----------------------------------------------------------------
NewAlarmDialog::NewAlarmDialog(QStringList invalidNames, QStringList invalidColors, QWidget * parent, Qt::WindowFlags flags)
: QDialog       {parent}
, m_invalidNames{invalidNames}
{
  setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowContextHelpButtonHint);
  setupUi(this);

  auto dateTime = QDateTime::currentDateTime();
  m_clock->setDate(dateTime.date());
  m_clock->setTime(dateTime.time());
  m_clock->setEnabled(false);

  m_timerRadio->setChecked(true);
  m_timerRadio->setAutoExclusive(true);

  m_showTray->setVisible(QSystemTrayIcon::isSystemTrayAvailable());

  m_colors = QColor::colorNames();
  for(auto color: invalidColors)
  {
    m_colors.removeAll(color);
  }

  for(auto color: m_colors)
  {
    auto pixmap = new QImage(32, 32, QImage::Format_RGB32);
    QPainter painter(pixmap);
    painter.fillRect(0,0,31,31, color);
    painter.end();
    m_colorComboBox->insertItem(m_colors.indexOf(color), QIcon(QPixmap::fromImage(*pixmap)), color);
  }
  m_colorComboBox->setCurrentIndex(0);

  m_soundComboBox->insertItems(0, sounds);
  m_soundComboBox->setCurrentIndex(0);

  connectSignals();

  loadSounds();

  m_timer->setMinimumTime(QTime(0,1,0));
  m_clock->setMinimumTime(QTime(0,1,0));

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

  checkOkButtonRequirements();
}

//-----------------------------------------------------------------
void NewAlarmDialog::onClockRadioToggled(bool value)
{
  m_clock->setEnabled(value);

  checkOkButtonRequirements();
}

//-----------------------------------------------------------------
void NewAlarmDialog::checkOkButtonRequirements()
{
  auto validName = !m_name->text().isEmpty() && !m_invalidNames.contains(m_name->text());
  auto validMessage = !m_message->text().isEmpty();
  auto validClock = m_clockRadio->isChecked() && (m_clock->dateTime() > QDateTime::currentDateTime());
  auto validTime = m_timerRadio->isChecked();

  auto valid = validName && validMessage && (validTime || validClock);

  m_buttons->button(QDialogButtonBox::Ok)->setEnabled(valid);
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

  connect(m_clock, SIGNAL(dateTimeChanged(const QDateTime &)),
          this,    SLOT(checkOkButtonRequirements()));

  connect(m_playSoundButton, SIGNAL(pressed()),
          this,              SLOT(playSound()));
}

//-----------------------------------------------------------------
void NewAlarmDialog::setPlayButtonIcon()
{
  auto sound = qobject_cast<QSoundEffect *>(sender());
  if(sound->isPlaying())
  {
    m_playSoundButton->setIcon(QIcon(":/MultiAlarm/stop.svg"));
  }
  else
  {
    m_playSoundButton->setIcon(QIcon(":/MultiAlarm/play.svg"));
  }
}

//-----------------------------------------------------------------
void NewAlarmDialog::setName(const QString& name)
{
  m_name->setText(name);
}

//-----------------------------------------------------------------
const QString NewAlarmDialog::name() const
{
  return m_name->text();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setMessage(const QString& message)
{
  m_message->setText(message);
}

//-----------------------------------------------------------------
const QString NewAlarmDialog::message() const
{
  return m_message->text();
}

//-----------------------------------------------------------------
bool NewAlarmDialog::isTimer() const
{
  return m_timerRadio->isChecked();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setTimerTime(const QTime& time)
{
  m_timer->setTime(time);
}

//-----------------------------------------------------------------
const QTime NewAlarmDialog::timerTime() const
{
  return m_timer->time();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setTimerLoop(bool value)
{
  m_timerLoop->setChecked(value);
}

//-----------------------------------------------------------------
bool NewAlarmDialog::timerLoop() const
{
  return m_timerLoop->isChecked();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setClockDateTime(const QDateTime& time)
{
  m_clock->setDateTime(time);
}

//-----------------------------------------------------------------
const QDateTime NewAlarmDialog::clockDateTime() const
{
  return m_clock->dateTime();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setColor(const QString& colorname)
{
  int index = 0;
  if(m_colors.contains(colorname))
  {
    index = m_colors.indexOf(colorname);
  }

  m_colorComboBox->setCurrentIndex(index);
}

//-----------------------------------------------------------------
const QString NewAlarmDialog::color() const
{
  return m_colors.at(m_colorComboBox->currentIndex());
}

//-----------------------------------------------------------------
void NewAlarmDialog::setSound(int soundIndex)
{
  soundIndex = soundIndex % m_soundComboBox->count();

  m_soundComboBox->setCurrentIndex(soundIndex);
}

//-----------------------------------------------------------------
int NewAlarmDialog::sound() const
{
  return m_soundComboBox->currentIndex();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setShowInTray(bool value)
{
  m_showTray->setChecked(value);
}

//-----------------------------------------------------------------
bool NewAlarmDialog::showInTray() const
{
  return m_showTray->isChecked();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setShowInDesktop(bool value)
{
  m_showDesktop->setChecked(value);
}

//-----------------------------------------------------------------
bool NewAlarmDialog::showInDesktop() const
{
  return m_showDesktop->isChecked();
}

//-----------------------------------------------------------------
void NewAlarmDialog::loadSounds()
{
  // NOTE: Load sound files. QSound can't play a file from the qt resource file
  // so we will dump them first to the temporal directory, then load the resources.
  for(auto i: {0,1,2})
  {
    auto file = QTemporaryFile::createLocalFile(soundNames[i]);
    m_sounds.insert(i, new QSoundEffect(this));
    m_sounds[i]->setSource(QUrl::fromLocalFile(file->fileName()));

    m_temporaryFiles << file;

    connect(m_sounds[i], SIGNAL(playingChanged()),
            this,        SLOT(setPlayButtonIcon()));
  }
}
