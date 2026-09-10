/*
 File: MultiAlarm.cpp
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
#include <MultiAlarm.h>
#include <AboutDialog.h>
#include <NewAlarmDialog.h>
#include <SettingsDialog.h>
#include <LogiLED.h>
#include <Alarm.h>

// Qt
#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QDir>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>

// C++
#include <functional>
#include <wingdi.h>
#include <memory>

const int MAX_HEIGHT = 800;
const int BAR_WIDTH  = 15;

const QString STATE           = "State";
const QString GEOMETRY        = "Geometry";
const QString ALARMS          = "Alarms";
const QString UPDATES         = "Update check frequency";
const QString LAST_CHECK      = "Last update check";
const QString CLOSE_IS_EXIT   = "Close is exit";
const QString RAISE_ON_FINISH = "Show main dialog on finish";

const QString ALARM_POSITION        = "Position";
const QString ALARM_MESSAGE         = "Message";
const QString ALARM_COLOR           = "Color";
const QString ALARM_IS_TIMER        = "Timer";
const QString ALARM_TIMER_LOOP      = "Loops";
const QString ALARM_TIMER_TIME      = "TimerTime";
const QString ALARM_CLOCK_DATETIME  = "ClockDateTime";
const QString ALARM_SOUND           = "Sound";
const QString ALARM_SOUND_VOLUME    = "SoundVolume";
const QString ALARM_USE_TRAY        = "UseTray";
const QString ALARM_USE_DESKTOP     = "UseDesktop";
const QString ALARM_USE_LOGILED     = "UseLogiled";
const QString ALARM_WIDGET_POSITION = "DesktopWidgetPosition";
const QString ALARM_WIDGET_OPACITY  = "DesktopWidgetOpacity";
const QString ALARM_CLOSE_SECONDS   = "AlarmCloseSeconds";

const QString INI_FILENAME = "MultiAlarm.ini";

const QString RELEASES_DATA = "https://api.github.com/repos/FelixdelasPozas/MultiAlarm/releases";
const QString RELEASES_ADDRESS = "https://github.com/FelixdelasPozas/MultiAlarm/releases";

//-----------------------------------------------------------------
MultiAlarm::MultiAlarm(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow{parent, flags}
, m_needsExit{false}
, m_netManager{std::make_unique<QNetworkAccessManager>(this)}
{
  const auto icon = appropiateTrayIcon();
  m_icon = new QSystemTrayIcon(icon);
  setWindowIcon(icon);

  setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

  setupUi(this);

  setFixedHeight(m_newButton->height() + menubar->height());
  centralWidget()->layout()->setContentsMargins(0,0,0,0);
  centralWidget()->layout()->setSpacing(0);

  m_scrollArea->hide();
  m_scrollArea->setAcceptDrops(true);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_scrollArea->verticalScrollBar()->hide();

  restoreSettings();

  setupTrayIcon();

  LogiLED::getInstance();

  connectSignals();

  checkForUpdates();
}

//-----------------------------------------------------------------
MultiAlarm::~MultiAlarm()
{
  saveSettings();
  m_updatesTimer.stop();
}

//-----------------------------------------------------------------
QStringList MultiAlarm::usedNames() const
{
  QStringList result;

  for(auto alarm: m_alarms)
    result << alarm->name();

  return result;
}

//-----------------------------------------------------------------
QStringList MultiAlarm::usedColors() const
{
  QStringList result;

  for(auto alarm: m_alarms)
    result << alarm->color();

  return result;
}

//-----------------------------------------------------------------
void MultiAlarm::createNewAlarm()
{
  NewAlarmDialog dialog(usedNames(), usedColors(), m_scrollArea);

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
  {
    auto alarmWidget = createAlarmWidget(dialog);
    m_alarms << alarmWidget;
    addAlarmWidget(alarmWidget);
  }
}

//-----------------------------------------------------------------
void MultiAlarm::aboutDialog()
{
  AboutDialog dialog(this);
  dialog.exec();
}

//-----------------------------------------------------------------
void MultiAlarm::settingsDialog()
{
  SettingsDialog dialog(m_configuration, this);
  dialog.exec();
}

//-----------------------------------------------------------------
void MultiAlarm::changeEvent(QEvent* e)
{
  if (e->type() == QEvent::WindowStateChange)
  {
    if (isMinimized())
    {
      hide();

      m_icon->show();
      e->ignore();
    }
  }

  QMainWindow::changeEvent(e);
}

//-----------------------------------------------------------------
void MultiAlarm::closeEvent(QCloseEvent *e)
{
    if (m_configuration.closeIsExit) {
        m_needsExit = true;
    }

    if (!m_needsExit) {
        hide();
        m_icon->show();

        e->accept();
    } else {
        if (e)
            QMainWindow::closeEvent(e);
        QApplication::exit(0);
    }
}

//-----------------------------------------------------------------
void MultiAlarm::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if(reason == QSystemTrayIcon::DoubleClick)
  {
    m_icon->hide();
    showNormal();
  }
}

//-----------------------------------------------------------------
void MultiAlarm::setupTrayIcon()
{
  auto menu = new QMenu();

  auto restore = new QAction(QIcon(":/MultiAlarm/application.svg"), tr("Restore"), this);
  connect(restore, SIGNAL(triggered()),
          this,    SLOT(onRestoreActionActivated()));

  auto newAlarm = new QAction(QIcon(":/MultiAlarm/add.ico"), tr("New Alarm..."), this);
  connect(newAlarm, SIGNAL(triggered()),
          this,    SLOT(createNewAlarm()));

  auto about = new QAction(QIcon(":/MultiAlarm/information.svg"), tr("About..."), this);
  connect(about, SIGNAL(triggered()),
          this,  SLOT(aboutDialog()));

  auto quit = new QAction(QIcon(":/MultiAlarm/exit.svg"), tr("Quit"), this);
  connect(quit, SIGNAL(triggered()),
          this, SLOT(onQuitActionActivated()));

  menu->addAction(newAlarm);
  menu->addAction(restore);
  menu->addSeparator();
  menu->addAction(about);
  menu->addAction(quit);

  m_icon->setContextMenu(menu);
  m_icon->setToolTip(tr("MultiAlarm"));
  m_icon->hide();
}

//-----------------------------------------------------------------
void MultiAlarm::onRestoreActionActivated()
{
  m_icon->hide();
  showNormal();
}

//-----------------------------------------------------------------
void MultiAlarm::onQuitActionActivated()
{
  m_needsExit = true;
  if (this->isVisible())
    close();
  else
    closeEvent(nullptr);
}

//-----------------------------------------------------------------
void MultiAlarm::addAlarmWidget(AlarmWidget *widget)
{
  if(!widget) return;

  m_scrollArea->show();

  auto oldHeight = currentHeight();
  auto barEnabled = (oldHeight > MAX_HEIGHT);

  connect(widget, SIGNAL(deleteAlarm()),
          this,   SLOT(onAlarmDeleted()));

  connect(widget, SIGNAL(finished()), 
          this, SLOT(onAlarmFinished()));

  auto layout = qobject_cast<QVBoxLayout*>(m_scrollArea->widget()->layout());
  layout->insertWidget(layout->count(), widget);

  auto height = currentHeight();
  auto needBar = (height > MAX_HEIGHT);

  if(barEnabled != needBar)
  {
    auto bar = m_scrollArea->verticalScrollBar();
    bar->show();

    height = MAX_HEIGHT;
    setFixedWidth(size().width() + BAR_WIDTH);
  }

  setFixedHeight(height);
}

//-----------------------------------------------------------------
void MultiAlarm::restoreSettings()
{
  const auto settings = applicationSettings();

  if(settings->contains(STATE))
  {
    auto state = settings->value(STATE).toByteArray();
    restoreState(state);
  }

  if(settings->contains(GEOMETRY))
  {
    auto geometry = settings->value(GEOMETRY).toByteArray();
    restoreGeometry(geometry);
  }

  m_configuration.closeIsExit = settings->value(CLOSE_IS_EXIT, false).toBool();
  m_configuration.update = static_cast<Update>(settings->value(UPDATES, 0).toInt());
  m_configuration.raiseOnFinish = settings->value(RAISE_ON_FINISH, false).toBool();
  m_configuration.lastCheck = settings->value(LAST_CHECK, QDateTime()).toDateTime();

  QStringList expired;

  settings->beginGroup(ALARMS);
  auto alarmIds = settings->childGroups();
  for(auto &alarmName : alarmIds)
  {
    auto alarmWidget = createAlarmWidget(*settings, alarmName);

    if(alarmWidget)
      m_alarms << alarmWidget;
    else
    {
      expired << alarmName;
      delete alarmWidget;
    }
  }
  settings->endGroup();

  if(!expired.empty())
  {
    auto message = QString("The following clock alarms will be deleted because they have expired:\n");
    for(auto alarm: expired)
      message += alarm + QString("\n");

    QMessageBox mb;
    mb.setWindowTitle("Expired Clock Alarms");
    mb.setWindowIcon(QIcon(":/MultiAlarm/application.ico"));
    mb.setText(message);
    mb.exec();
  }

  auto sorter = [](const AlarmWidget *l, const AlarmWidget *r)
  {
    return (l->alarmConfiguration().position < r->alarmConfiguration().position);
  };
  std::sort(m_alarms.begin(), m_alarms.end(), sorter);

  for (auto alarm : m_alarms) {
      addAlarmWidget(alarm);
  }
}

//-----------------------------------------------------------------
void MultiAlarm::saveSettings() const
{
  auto settings = applicationSettings();
  settings->clear();

  settings->setValue(STATE, saveState());
  settings->setValue(GEOMETRY, saveGeometry());

  settings->setValue(CLOSE_IS_EXIT, m_configuration.closeIsExit);
  settings->setValue(UPDATES, static_cast<int>(m_configuration.update));
  settings->setValue(RAISE_ON_FINISH, m_configuration.raiseOnFinish);
  settings->setValue(LAST_CHECK, m_configuration.lastCheck);

  settings->beginGroup(ALARMS);

  if(!m_alarms.empty())
  {
    auto layout = m_scrollArea->widget()->layout();
    for(auto i = 0; i < layout->count(); ++i)
    {
      auto item = layout->itemAt(i);
      auto widget = qobject_cast<AlarmWidget*>(item->widget());
      auto conf = widget->alarmConfiguration();

      settings->beginGroup(widget->name());

      settings->setValue(ALARM_POSITION, i);
      settings->setValue(ALARM_MESSAGE, conf.message);
      settings->setValue(ALARM_COLOR, widget->color());
      settings->setValue(ALARM_IS_TIMER, conf.isTimer);

      if(conf.isTimer)
      {
        settings->setValue(ALARM_TIMER_LOOP, conf.timerLoops);
        settings->setValue(ALARM_TIMER_TIME, conf.timerTime);
      }
      else
        settings->setValue(ALARM_CLOCK_DATETIME, conf.clockDateTime);

      settings->setValue(ALARM_SOUND, conf.sound);
      settings->setValue(ALARM_SOUND_VOLUME, conf.soundVolume);
      settings->setValue(ALARM_USE_TRAY, conf.useTray);
      settings->setValue(ALARM_USE_DESKTOP, conf.useDesktopWidget);
      settings->setValue(ALARM_USE_LOGILED, conf.useLogiled);
      settings->setValue(ALARM_WIDGET_POSITION, conf.widgetPosition);
      settings->setValue(ALARM_WIDGET_OPACITY, conf.widgetOpacity);
      settings->setValue(ALARM_CLOSE_SECONDS, conf.closeSeconds);

      settings->endGroup();
    }
  }

  settings->endGroup();
  settings->sync();
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmDeleted()
{
  AlarmWidget* widget = qobject_cast<AlarmWidget *>(sender());
  if(!widget) return;

  auto oldHeight = currentHeight();
  auto barEnabled = (oldHeight > MAX_HEIGHT);

  m_scrollWidget->layout()->removeWidget(widget);
  m_alarms.removeOne(widget);
  widget->deleteLater();

  auto newHeight = currentHeight();
  auto needBar = (newHeight > MAX_HEIGHT);

  if(barEnabled != needBar)
  {
    auto bar = m_scrollArea->verticalScrollBar();
    bar->hide();

    setFixedWidth(size().width() - BAR_WIDTH);
  }

  if(needBar)
    setFixedHeight(MAX_HEIGHT);
  else
    setFixedHeight(newHeight);

  if(m_alarms.empty())
    m_scrollArea->hide();
}

//-----------------------------------------------------------------
void MultiAlarm::onAlarmFinished()
{
  if(m_configuration.raiseOnFinish && (isMinimized() || m_icon->isVisible()))
  {
    onRestoreActionActivated();
  }
}

//-----------------------------------------------------------------
AlarmWidget* MultiAlarm::createAlarmWidget(const NewAlarmDialog& dialog)
{
  AlarmConfiguration conf;
  conf.name    = dialog.name();
  conf.message = dialog.message();
  conf.color   = dialog.color().toLower();
  conf.isTimer = dialog.isTimer();

  if(conf.isTimer)
  {
    conf.timerTime  = dialog.timerTime();
    conf.timerLoops = dialog.timerLoop();
  }
  else
    conf.clockDateTime = dialog.clockDateTime();

  conf.sound            = dialog.sound();
  conf.soundVolume      = dialog.soundVolume();
  conf.useTray          = dialog.showInTray();
  conf.useDesktopWidget = dialog.showInDesktop();
  conf.useLogiled       = dialog.showInKeyboard();
  conf.widgetPosition   = dialog.desktopWidgetPosition();
  conf.widgetOpacity    = dialog.widgetOpacity();
  conf.closeSeconds     = dialog.closeSeconds();

  auto widget = new AlarmWidget(this);
  widget->setConfiguration(conf);

  return widget;
}

//-----------------------------------------------------------------
AlarmWidget* MultiAlarm::createAlarmWidget(QSettings &settings, const QString &name)
{
  settings.beginGroup(name);

  AlarmConfiguration conf;
  conf.name     = name;
  conf.message  = settings.value(ALARM_MESSAGE, QString()).toString();
  conf.color    = settings.value(ALARM_COLOR, QString("white")).toString();
  conf.isTimer  = settings.value(ALARM_IS_TIMER, false).toBool();
  conf.position = settings.value(ALARM_POSITION, 0).toInt();

  if(conf.isTimer)
  {
    conf.timerTime  = settings.value(ALARM_TIMER_TIME, 60).toULongLong();
    conf.timerLoops = settings.value(ALARM_TIMER_LOOP, false).toBool();
  }
  else
    conf.clockDateTime = settings.value(ALARM_CLOCK_DATETIME, QDateTime()).toDateTime();

  conf.sound            = settings.value(ALARM_SOUND, 0).toInt();
  conf.soundVolume      = settings.value(ALARM_SOUND_VOLUME, 100).toInt();
  conf.useTray          = settings.value(ALARM_USE_TRAY, false).toBool();
  conf.useDesktopWidget = settings.value(ALARM_USE_DESKTOP, false).toBool();
  conf.useLogiled       = settings.value(ALARM_USE_LOGILED, false).toBool();
  conf.widgetPosition   = settings.value(ALARM_WIDGET_POSITION, QPoint{0,0}).toPoint();
  conf.widgetOpacity    = settings.value(ALARM_WIDGET_OPACITY, 60).toInt();
  conf.closeSeconds     = settings.value(ALARM_CLOSE_SECONDS, 0).toInt();

  settings.endGroup();

  if(!conf.isTimer && conf.clockDateTime < QDateTime::currentDateTime())
    return nullptr;

  auto widget = new AlarmWidget(this);
  widget->setConfiguration(conf);

  return widget;
}

//-----------------------------------------------------------------
int MultiAlarm::currentHeight() const
{
  const auto alarmSize = m_alarms.size() * m_newButton->size().height();

  return alarmSize + m_newButton->size().height() + menubar->height() + 4; // 4 is spacing in the bottom.
}

//-----------------------------------------------------------------
void MultiAlarm::connectSignals()
{
  connect(m_quitAction, SIGNAL(triggered()),
          QApplication::instance(), SLOT(quit()));

  connect(m_newAction, SIGNAL(triggered()),
          this,        SLOT(createNewAlarm()));

  connect(m_newButton, SIGNAL(clicked(bool)),
          this,        SLOT(createNewAlarm()));

  connect(m_aboutAction, SIGNAL(triggered()),
          this,          SLOT(aboutDialog()));

  connect(m_settingsAction, SIGNAL(triggered()),
          this,             SLOT(settingsDialog()));

  connect(m_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this,   SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));

  connect(m_netManager.get(), SIGNAL(finished(QNetworkReply*)),
          this,               SLOT(replyFinished(QNetworkReply*)));
}

//-----------------------------------------------------------------
std::unique_ptr<QSettings> MultiAlarm::applicationSettings() const
{
  QDir applicationDir{QCoreApplication::applicationDirPath()};
  if(applicationDir.exists(INI_FILENAME))
  {
    const auto fInfo = QFileInfo(applicationDir.absoluteFilePath(INI_FILENAME));
    if(fInfo.isWritable()) {
      return std::make_unique<QSettings>(INI_FILENAME, QSettings::IniFormat);
    }
  }

  return std::make_unique<QSettings>("Felix de las Pozas Alvarez", "MultiAlarm");
}

//-----------------------------------------------------------------
QIcon MultiAlarm::appropiateTrayIcon() const
{
  // Returns the squared distance to avoid a slow square root function
  std::function<double(const COLORREF&, const COLORREF&)> euclideanDistanceSquared = [](const COLORREF& c1,
                                                                                        const COLORREF& c2) {
    const int r1 = (int)GetRValue(c1);
    const int g1 = (int)GetGValue(c1);
    const int b1 = (int)GetBValue(c1);
    const int r2 = (int)GetRValue(c2);
    const int g2 = (int)GetGValue(c2);
    const int b2 = (int)GetBValue(c2);

    long long dr = r1 - r2;
    long long dg = g1 - g2;
    long long db = b1 - b2;
    return dr * dr + dg * dg + db * db;
  };

  auto euclideanDistance = [&euclideanDistanceSquared](const COLORREF& c1, const COLORREF& c2) {
      return std::sqrt(euclideanDistanceSquared(c1, c2));
  };

  // 1. Check if the taskbar uses the accent color
  // ColorPrevalence = 1 means Accent Color is enabled on Start, Taskbar, and Action Center
  DWORD colorPrevalence = 0;
  ReadRegistryDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                    L"ColorPrevalence", colorPrevalence);

  // 2. Check if Apps use Light or Dark theme (for fallback)
  DWORD appsUseLightTheme = 0;
  ReadRegistryDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                    L"AppsUseLightTheme", appsUseLightTheme);

  COLORREF taskbarColor;

  if (colorPrevalence == 1) {
      // Read the actual Accent Color chosen by the user
      DWORD accentColor = 0;
      if (ReadRegistryDword(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                            L"AccentColorMenu", accentColor)) {
          // AccentColorMenu is stored as ABGR. Convert to standard RGB or COLORREF
          BYTE r = GetBValue(accentColor); // Windows swaps R and B in this registry layout
          BYTE g = GetGValue(accentColor);
          BYTE b = GetRValue(accentColor);
          taskbarColor = RGB(r, g, b);
      } else {
          // Fallback if AccentColorMenu registry read fails
          taskbarColor = RGB(0, 120, 215); // Default Windows Blue
      }
  } else {
      // Fallback: Taskbar uses the default theme color (Dark or Light)
      if (appsUseLightTheme == 1) {
          taskbarColor = RGB(243, 243, 243); // Default Windows Light Taskbar Gray
      } else {
          taskbarColor = RGB(16, 16, 16); // Default Windows Dark Taskbar Black
      }
  }

  const auto blackDistance = euclideanDistance(taskbarColor, RGB(0, 0, 0));
  const auto whiteDistance = euclideanDistance(taskbarColor, RGB(255, 255, 255));

  if (whiteDistance > blackDistance) {
      return QIcon(":/MultiAlarm/application_w.svg");
  }

  return QIcon(":/MultiAlarm/application.svg");
}

//--------------------------------------------------------------------
void MultiAlarm::checkForUpdates()
{
  m_updatesTimer.stop();

  auto last = m_configuration.lastCheck;

  switch(m_configuration.update)
  {
    case Update::MONTHLY:
      if(last.isValid())
        last = last.addMonths(1);
      break;
    case Update::WEEKLY:
      if(last.isValid())
        last = last.addDays(7);
      break;
    case Update::DAILY:
      if(last.isValid())
        last = last.addDays(1);
      break;
    default:
    case Update::NEVER:
      return;
      break;
  }

  const auto now = QDateTime::currentDateTime();
  if(last.isValid() && last > now)
  {
    if(m_updatesTimer.isActive()) m_updatesTimer.stop();
    auto msec = now.msecsTo(last);
    m_updatesTimer.singleShot(msec, SLOT(checkForUpdates));
  }
  else
  {
    m_netManager->get(QNetworkRequest{QUrl{RELEASES_DATA}});

    m_configuration.lastCheck = now;
    checkForUpdates();
  }
}

//--------------------------------------------------------------------
void MultiAlarm::replyFinished(QNetworkReply *reply)
{
  const auto originUrl = reply->request().url().toString();

  if(originUrl.contains("github", Qt::CaseInsensitive))
  {
    if(reply->error() == QNetworkReply::NoError)
    {
      const auto contents = reply->readAll();
      processGithubData(contents);
    }
    else
    {
      showMessageBox(QMessageBox::Critical, tr("Unable to download update file."), reply->errorString(), tr("MultiAlarm updates"));
    }
  }

  reply->deleteLater();
}

//--------------------------------------------------------------------
void MultiAlarm::processGithubData(const QByteArray &data)
{
  const auto jsonDocument = QJsonDocument::fromJson(data);

  int currentNumbers[3], lastNumbers[3];
  const auto currentVersion = AboutDialog::VERSION.split(".");

  // Github parse tag of last release.
  auto jsonObj = jsonDocument.array();
  auto lastRelease = jsonObj.at(0).toObject();
  const auto version = lastRelease.value("tag_name").toString();
  const auto lastVersion = version.split(".");
  const auto body = lastRelease.value("body").toString();
  bool hasError = false;

qDebug() << lastVersion << body;
  if(lastVersion.size() != 3 || body.isEmpty())
  {
    hasError = true;
  }
  else
  {
    bool ok = false;
    for(int i: {0,1,2})
    {
      currentNumbers[i] = currentVersion.at(i).toInt(&ok);
      if(!ok) hasError = true;
      lastNumbers[i] = lastVersion.at(i).toInt(&ok);
      if(!ok) hasError = true;
      if(hasError) break;
    }
  }

  if(!hasError)
  {
    if((currentNumbers[0] < lastNumbers[0]) ||
      ((currentNumbers[0] == lastNumbers[0]) && (currentNumbers[1] < lastNumbers[1])) ||
      ((currentNumbers[0] == lastNumbers[0]) && (currentNumbers[1] == lastNumbers[1]) && currentNumbers[2] < lastNumbers[2]))
    {
      const auto message = tr("There is a new release of <b>MultiAlarm</b> at the <a href=\"https://github.com/FelixdelasPozas/MultiAlarm/releases\">github website</a>!");
      const auto informative = tr("<center><b>Version %1</b> has been released!</center>").arg(version);
      const auto details = tr("Release notes:\n%1").arg(body);
      const auto title = tr("MultiAlarm updated to version %1").arg(version);

      QMessageBox msgBox;
      msgBox.setWindowTitle(title);
      msgBox.setText(message);
      msgBox.setInformativeText(informative);
      msgBox.setDetailedText(details);
      msgBox.setWindowIcon(QIcon(":/MultiAlarm/application.svg"));
      msgBox.setIconPixmap(QIcon(":/MultiAlarm/application.svg").pixmap(QSize{64,64}));
      msgBox.exec();
    }
  }
}
