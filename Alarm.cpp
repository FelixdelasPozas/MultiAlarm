/*
		File: Alarm.cpp
    Created on: 04/07/2015
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
#include "Alarm.h"

//-----------------------------------------------------------------
Alarm::Alarm(unsigned long long seconds, bool loop)
: m_seconds            {seconds}
, m_loop               {loop}
, m_interval           {seconds/8.0}
, m_completed_intervals{0}
, m_completed_seconds  {0}
, m_remaining_seconds  {seconds}
{
  m_timer.setInterval(1000);
  m_timer.setSingleShot(false);

  connect(&m_timer, SIGNAL(timeout()),
          this,     SLOT(second()));
}

//-----------------------------------------------------------------
Alarm::~Alarm()
{
}

//-----------------------------------------------------------------
void Alarm::start()
{
  m_timer.start();
}
//-----------------------------------------------------------------
void Alarm::stop()
{
  m_timer.stop();

  m_completed_intervals = 0;
  m_completed_seconds = 0;
  m_remaining_seconds = m_seconds;
}

//-----------------------------------------------------------------
void Alarm::pause(bool paused)
{
  if(paused == m_timer.isActive())
  {
    switch (paused)
    {
      case true:
        stop();
        break;
      case false:
      default:
        start();
        break;
    }
  }
}

//-----------------------------------------------------------------
unsigned int Alarm::progress() const
{
  return 100 * (m_completed_seconds/static_cast<double>(m_seconds));
}

//-----------------------------------------------------------------
unsigned int Alarm::intervals() const
{
  return m_completed_intervals;
}

//-----------------------------------------------------------------
unsigned long long Alarm::seconds() const
{
  return m_completed_seconds;
}

//-----------------------------------------------------------------
const QTime Alarm::time() const
{
  QTime time(0,0,0);
  return time.addSecs(m_completed_seconds);
}

//-----------------------------------------------------------------
const QTime Alarm::remainingTime() const
{
  QTime time(0,0,0);
  return time.addSecs(m_seconds - m_completed_seconds);
}

//-----------------------------------------------------------------
void Alarm::second()
{
  ++m_completed_seconds;
  --m_remaining_seconds;

  emit tic(m_remaining_seconds);

  if((m_interval * (m_completed_intervals + 1)) <= m_completed_seconds)
  {
    ++m_completed_intervals;

    emit interval();
  }

  if(m_completed_seconds == m_seconds)
  {
    emit timeout();

    if (!m_loop)
    {
      m_timer.stop();

      disconnect(&m_timer, SIGNAL(timeout()),
                 this,     SLOT(second()));
    }
    else
    {
      m_completed_seconds = 0;
      m_completed_intervals = 0;
    }
  }
}
