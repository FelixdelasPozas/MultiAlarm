/*
		File: Alarm.h
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

#ifndef ALARM_H_
#define ALARM_H_

// Qt
#include <QTimer>
#include <QTime>

/** \class Alarm
 * \brief Holds alarm timer.
 *
 */
class Alarm
: public QObject
{
    Q_OBJECT
  public:
    struct AlarmTime
    {
      int days;
      int hours;
      int minutes;
      int seconds;

      explicit AlarmTime(int d, int h, int m, int s): days{d}, hours{h}, minutes{m}, seconds{s} {};
    };

    /** \brief Alarm class constructor.
     * \param[in] seconds duration of the alarm in seconds.
     * \param[in] loop true to loop the alarm.
     * \param[in] parent raw pointer of the QObject parent of this one.
     *
     */
    explicit Alarm(AlarmTime time, bool loop);

    /** \brief Alarm class virtual destructor.
     *
     */
    virtual ~Alarm();

    /** \brief Starts the alarm.
     *
     */
    void start();

    /** \brief Stops the alarm.
     *
     */
    void stop();

    /** \brief Pauses/resumes the alarm.
     * \param[in] paused true to pause and false otherwise.
     *
     */
    void pause(bool paused);

    /** \brief Returns the progress of the alarm in the range [0-100].
     *
     */
    unsigned int progress() const;

    /** \brief Returns the number of completed 1/8th intervals of the alarm completed.
     *
     */
    unsigned int intervals() const;

    /** \brief Returns the remaining time of the alarm.
     *
     */
    const AlarmTime remainingTime() const;

  signals:
    /** \brief Signal launched every second.
     *
     */
    void tic();

    /** \brief Signal launched every completed interval. (1/8 th of the duration of the alarm).
     *
     */
    void interval();

    /** \brief Signal launched at the end of the alarm.
     *
     */
    void timeout();

  private slots:
    /** \brief Updates the alarm internal values.
     *
     */
    void second();

  private:
    AlarmTime          m_time;          /** duration of the timer/clock.                */
    AlarmTime          m_remainingTime; /** remaining time of the timer/clock           */
    bool               m_loop;          /** true to restart the alarm once it finishes. */
    unsigned int       m_intervals;     /** number of completed intervals.              */
    QTimer             m_timer;         /** timer object.                               */
};

#endif // ALARM_H_
