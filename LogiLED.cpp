/*
 File: LogiLED.cpp
 Created on: 21/11/2018
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
#include <LogiLED.h>

// Logitech gaming SDK
extern "C"
{
  #include <LogitechLEDLib.h>
}

// Qt
#include <QTimer>
#include <QReadLocker>
#include <QWriteLocker>
#include <QListIterator>

// C++
#include <chrono>
#include <thread>

const QList<LogiLed::KeyName> KEYS { LogiLed::F1, LogiLed::F2, LogiLed::F3, LogiLed::F4, LogiLed::F5, LogiLed::F6, LogiLed::F7,
                                     LogiLed::F8, LogiLed::F9, LogiLed::F10, LogiLed::F11, LogiLed::F12, LogiLed::PRINT_SCREEN,
                                     LogiLed::SCROLL_LOCK, LogiLed::PAUSE_BREAK };

using namespace LogiLed;

//--------------------------------------------------------------------
LogiLED::LogiLED()
: m_available{LogiLedInitWithName("MultiAlarm")}
, m_current  {0}
{
  if(m_available)
    LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB);
}

//--------------------------------------------------------------------
LogiLED::~LogiLED()
{
  LogiLedShutdown();
}

//--------------------------------------------------------------------
LogiLED& LogiLED::getInstance()
{
  static LogiLED instance;

  return instance;
}

//--------------------------------------------------------------------
bool LogiLED::isAvailable()
{
  return getInstance().isInitialized();
}

//--------------------------------------------------------------------
bool LogiLED::registerItem(const QString& id, const int progress, const QColor& foreground, const QColor& background)
{
  if(m_available)
  {
    const Item item{id, progress, foreground, background};
    int isEmpty = true;
    {
      QWriteLocker lock(&m_lock);
      isEmpty = m_items.isEmpty();

      if(!m_items.contains(item))
        m_items << item;
    }

    // only start updating lights when inserting the first
    if (isEmpty)
      updateLights();

    return true;
  }
  return false;
}

//--------------------------------------------------------------------
bool LogiLED::unregisterItem(const QString& id)
{
  if(m_available)
  {
    QWriteLocker lock(&m_lock);

    for(const auto &item: m_items)
    {
      if(item.id == id)
      {
        m_items.removeAll(item);

        if(m_items.isEmpty())
          restart();

        return true;
      }
    }
  }

  return false;
}

//--------------------------------------------------------------------
bool LogiLED::updateItem(const QString& id, const int progress, const QColor& foreground, const QColor& background)
{
  if(m_available)
  {
    QWriteLocker lock(&m_lock);

    for(int i = 0; i < m_items.size(); ++i)
    {
      auto item = m_items.at(i);

      if(item.id == id)
      {
        item.progress = progress;
        if(foreground != QColor()) item.foreground = foreground;
        if(background != QColor()) item.background = background;

        if(m_current == i || item.id == "NewAlarm")
          changeKeysToColor(item.progress, item.foreground, item.background);

        return true;
      }
    }
  }

  return false;
}

//--------------------------------------------------------------------
void LogiLED::updateLights()
{
  QReadLocker lock(&m_lock);

  if(m_available && !m_items.isEmpty())
  {
    ++m_current;
    m_current = m_current % m_items.size();
    auto &item = m_items.at(m_current);

    changeKeysToColor(item.progress, item.foreground, item.background);

    QTimer::singleShot(5000, this, SLOT(updateLights()));
  }
}

//--------------------------------------------------------------------
void LogiLED::restart() const
{
  if(m_available)
  {
    // Apparently this is the only way to restore the default profile of the user, as there is no way
    // in the Logitech SDK to get the keys color (for us to store state) and the methods in the SDK
    // to store that info just don't work.
    LogiLedShutdown();
    LogiLedInitWithName("MultiAlarm");
  }
}

//--------------------------------------------------------------------
void LogiLED::changeKeysToColor(const int progress, const QColor foreground, const QColor background)
{
  const double KEY_PERCENT = 100./KEYS.size();

  double dProgress = progress;

  for(auto it = KEYS.begin(); it != KEYS.end(); ++it, dProgress -= KEY_PERCENT)
  {
    auto key = *it;
    if(dProgress < 0) dProgress = 0;
    auto percent = dProgress > KEY_PERCENT ? 1 : dProgress/KEY_PERCENT;
    LogiLedSetLightingForKeyWithKeyName(key, 100 * (foreground.redF() * percent + background.redF() * (1-percent)),
                                             100 * (foreground.greenF() * percent + background.greenF() * (1-percent)),
                                             100 * (foreground.blueF() * percent + background.blueF() * (1-percent)));
  }

}

//--------------------------------------------------------------------
std::string LogiLED::version() const
{
  int major, minor, build;
  LogiLedGetSdkVersion(&major, &minor, &build);

  return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build);
}

//--------------------------------------------------------------------
bool operator==(const LogiLED::Item &rhs, const LogiLED::Item &lhs)
{
  return rhs.id == lhs.id;
}
