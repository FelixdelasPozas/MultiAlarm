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
#include <DesktopWidget.h>
#include <NewAlarmDialog.h>
#include <LogiLED.h>

// Qt
#include <QImage>
#include <QPainter>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QTemporaryFile>
#include <QScreen>
#include <QSoundEffect>
#include <QSystemTrayIcon>

const QStringList soundFiles = { ":/MultiAlarm/sounds/Beeper 1.wav",
                                 ":/MultiAlarm/sounds/Beeper 2.wav",
                                 ":/MultiAlarm/sounds/Beeper 3.wav",
                                 ":/MultiAlarm/sounds/Buzzer 1.wav",
                                 ":/MultiAlarm/sounds/Buzzer 2.wav",
                                 ":/MultiAlarm/sounds/Code Red.wav",
                                 ":/MultiAlarm/sounds/Electronic.wav",
                                 ":/MultiAlarm/sounds/Pager.wav",
                                 ":/MultiAlarm/sounds/Smoke.wav" };

const QStringList soundNames = { "Beeper 1",
                                 "Beeper 2",
                                 "Beeper 3",
                                 "Buzzer 1",
                                 "Buzzer 2",
                                 "Code Red",
                                 "Electronic",
                                 "Pager",
                                 "Smoke Alarm" };

const QStringList defaultPositions = { "Top Left",
                                       "Top Center",
                                       "Top Right",
                                       "Center Left",
                                       "Center",
                                       "Center Right",
                                       "Bottom Left",
                                       "Bottom Center",
                                       "Bottom Right" };

//-----------------------------------------------------------------
NewAlarmDialog::NewAlarmDialog(QStringList invalidNames, QStringList invalidColors, QWidget * parent, Qt::WindowFlags flags)
: QDialog       {parent}
, m_invalidNames{invalidNames}
, m_widget      {true, this}
{
  setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowContextHelpButtonHint);
  setupUi(this);

  if(LogiLED::isAvailable())
  {
    m_showLogiled->setChecked(true);
  }
  else
  {
    m_showLogiled->setEnabled(false);
    m_showLogiled->setToolTip(tr("LogiLED is not available because LGHub or the Logitech Gaming Software is not running."));
  }

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
  m_widget.setColor(m_colors.at(0));

  m_soundComboBox->insertItems(0, soundNames);
  m_soundComboBox->setCurrentIndex(0);

  computeDesktopWidgetPositions();

  connectSignals();

  m_opacitySlider->setMinimum(0);
  m_opacitySlider->setMaximum(100);
  m_opacitySlider->setValue(60);

  m_volumeSlider->setMinimum(0);
  m_volumeSlider->setMaximum(100);
  m_volumeSlider->setValue(100);

  loadSounds();

  m_timer->setMinimumTime(QTime{0,1,0});
  m_clock->setMinimumTime(QTime{0,1,0});

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

  if(m_showLogiled->isChecked())
  {
    LogiLED::getInstance().unregisterItem("NewAlarm");
  }
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
void NewAlarmDialog::onDesktopWidgetStateChanged(int value)
{
  auto enabled = (value == Qt::Checked);

  m_positionLabel->setEnabled(enabled);
  m_positionComboBox->setEnabled(enabled);
  m_opacityLabel->setEnabled(enabled);
  m_opacitySlider->setEnabled(enabled);
  m_opacitySliderValue->setEnabled(enabled);

  value ? m_widget.show() : m_widget.hide();
}

//-----------------------------------------------------------------
void NewAlarmDialog::onOpacityValueChanged(int value)
{
  m_opacitySliderValue->setText(QString("%1%").arg(value));
  m_widget.setOpacity(value);
}

//-----------------------------------------------------------------
void NewAlarmDialog::onVolumeChanged(int value)
{
  m_volumeSliderValue->setText(QString("%1%").arg(value));
}
//-----------------------------------------------------------------
void NewAlarmDialog::onWidgetPositionChanged(int value)
{
  if(value != 0)
  {
    m_widget.setPosition(m_widgetPositions.at(value));
  }
}

//-----------------------------------------------------------------
void NewAlarmDialog::onColorChanged(int value)
{
  auto color = m_colors.at(value);

  m_widget.setColor(color);

  if(m_showLogiled->isChecked())
  {
    LogiLED::getInstance().updateItem("NewAlarm", 100, QColor{color});
  }
}

//-----------------------------------------------------------------
void NewAlarmDialog::checkOkButtonRequirements()
{
  auto validName = !m_name->text().isEmpty() && !m_invalidNames.contains(m_name->text());
  auto validMessage = !m_message->text().isEmpty();
  auto validClock = m_clockRadio->isChecked() && (m_clock->dateTime() > QDateTime::currentDateTime());
  auto validTime = m_timerRadio->isChecked();

  auto valid = validName && validMessage && (validTime || validClock);

  m_widget.setName(m_name->text());

  m_buttons->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

//-----------------------------------------------------------------
void NewAlarmDialog::playSound()
{
  for(int i = 0; i < soundFiles.size(); ++i)
  {
    if(m_sounds[i]->isPlaying())
    {
      m_sounds[i]->stop();
      return;
    }
  }

  m_sounds[m_soundComboBox->currentIndex()]->setVolume(m_volumeSlider->value()/100.0);
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

  connect(m_showDesktop, SIGNAL(stateChanged(int)),
          this,          SLOT(onDesktopWidgetStateChanged(int)));

  connect(m_opacitySlider, SIGNAL(valueChanged(int)),
          this,            SLOT(onOpacityValueChanged(int)));

  connect(m_positionComboBox, SIGNAL(currentIndexChanged(int)),
          this,            SLOT(onWidgetPositionChanged(int)));

  connect(m_colorComboBox, SIGNAL(currentIndexChanged(int)),
          this,            SLOT(onColorChanged(int)));

  connect(m_volumeSlider, SIGNAL(valueChanged(int)),
          this,           SLOT(onVolumeChanged(int)));

  connect(&m_widget, SIGNAL(beingDragged()),
          this,      SLOT(onWidgetBeingDragged()));

  connect(m_showLogiled, SIGNAL(stateChanged(int)),
          this,          SLOT(onKeyboardNotificationStateChanged(int)));
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
void NewAlarmDialog::onWidgetBeingDragged()
{
  m_positionComboBox->setCurrentIndex(0);
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
void NewAlarmDialog::setIsTimer(bool value)
{
  m_timerRadio->setChecked(value);
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
void NewAlarmDialog::setSoundVolume(int value)
{
  m_volumeSlider->setValue(value);
}

//-----------------------------------------------------------------
int NewAlarmDialog::soundVolume() const
{
  return m_volumeSlider->value();
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
void NewAlarmDialog::setShowInKeyboard(bool value)
{
  m_showLogiled->setChecked(value);
}

//-----------------------------------------------------------------
bool NewAlarmDialog::showInKeyboard() const
{
  return m_showLogiled->isChecked();
}

//-----------------------------------------------------------------
void NewAlarmDialog::setDesktopWidgetPosition(const QPoint &position)
{
  for(int i = 0; i < m_widgetPositions.size(); ++i)
  {
    if(position == m_widgetPositions.at(i))
    {
      m_positionComboBox->setCurrentIndex(i);
      return;
    }
  }
  m_positionComboBox->setCurrentIndex(0);
  m_widget.setPosition(position);
}

//-----------------------------------------------------------------
void NewAlarmDialog::setWidgetOpacity(int opacity)
{
  m_opacitySlider->setValue(opacity);
}

//-----------------------------------------------------------------
int NewAlarmDialog::widgetOpacity() const
{
  return m_opacitySlider->value();
}

//-----------------------------------------------------------------
const QPoint NewAlarmDialog::desktopWidgetPosition() const
{
  return m_widget.pos();
}

//-----------------------------------------------------------------
void NewAlarmDialog::loadSounds()
{
  // NOTE: Load sound files. QSound can't play a file from the qt resource file
  // so we will dump them first to the temporal directory, then load the resources.
  for(int i = 0; i < soundFiles.size(); ++i)
  {
    auto file = QTemporaryFile::createNativeFile(soundFiles[i]);
    m_sounds.insert(i, new QSoundEffect(this));
    m_sounds[i]->setSource(QUrl::fromLocalFile(file->fileName()));

    m_temporaryFiles << file;

    connect(m_sounds[i], SIGNAL(playingChanged()),
            this,        SLOT(setPlayButtonIcon()));
  }
}

//-----------------------------------------------------------------
void NewAlarmDialog::computeDesktopWidgetPositions()
{
  QStringList positionNames;

  positionNames << tr("Custom (drag to position)");
  m_widgetPositions << QPoint{0,0};

  const auto screens = QApplication::screens();
  computePositions(QRect{screens.at(0)->availableVirtualGeometry()}, "Global ", positionNames);

  for (int i = 0; i < screens.size(); ++i)
  {
    computePositions(screens.at(i)->availableGeometry(), QString("Monitor %1 ").arg(i), positionNames);
  }

  m_positionComboBox->insertItems(0, positionNames);
}

//-----------------------------------------------------------------
void NewAlarmDialog::computePositions(const QRect &rect, const QString &screenName, QStringList &positionNames)
{
  auto widgetSize = DesktopWidget::WIDGET_SIZE;

  for(int y: {rect.y(), rect.y()+(rect.height()-widgetSize)/2, rect.y()+rect.height()-widgetSize})
  {
    for(int x: {rect.x(), rect.x()+(rect.width()-widgetSize)/2, rect.x()+rect.width()-widgetSize})
    {
      m_widgetPositions << QPoint{x,y};
    }
  }

  for(auto position: defaultPositions)
  {
    positionNames << screenName + position;
  }
}

//-----------------------------------------------------------------
void NewAlarmDialog::onKeyboardNotificationStateChanged(int value)
{
  if(value)
  {
    auto color = m_colors.at(m_colorComboBox->currentIndex());
    LogiLED::getInstance().registerItem("NewAlarm", 100, QColor{color}, QColor{0,0,0});
  }
  else
  {
    LogiLED::getInstance().unregisterItem("NewAlarm");
  }
}
