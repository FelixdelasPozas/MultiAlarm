/*
 File: DurationSpinBox.h
 Created on: 04/09/2026
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

#ifndef _DURATION_SPIN_BOX_H
#define _DURATION_SPIN_BOX_H

// Qt
#include <QAbstractSpinBox>
#include <QtGlobal>

/** \class DurationSpinBox
 * \brief Implements a timer spinbox that contains the duration including days.
 */
class DurationSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
  public:
    /** \brief DurationSpinBox class constructor.
     * \param[in] parent Raw pointer of the widget parent of this one. 
     */
    explicit DurationSpinBox(QWidget* parent = nullptr);

    virtual ~DurationSpinBox() = default;

    /** \brief Returns the current duration in seconds. 
     */
    quint64 durationInSeconds() const
    {
        return m_totalSeconds;
    }

    /** \brief Sets the duration of the timer. 
     * \param[in] seconds Duration in seconds. 
     */
    void setDurationInSeconds(quint64 seconds);


    void stepBy(int steps) override;
    QValidator::State validate(QString& input, int& pos) const override;
    void fixup(QString& input) const override;

  signals:
    void durationChanged(quint64 totalSeconds);

  protected:
    StepEnabled stepEnabled() const override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

  private:
    enum class Section /** sections of the input widgets, parts of the timer. */
    {
        None = 0,
        Days,
        Hours,
        Minutes,
        Seconds
    };

    /** \brief Helper method to determine the position of the cursor. 
     * Format DD:HH:MM:SS (Positions 01 2 34 5 67 8 910). 
     */
    Section currentSection() const;

    /** \brief Helper method to select the current two digits where the cursor is. 
     * \param[in] sec Section 
     */
    void selectSection(Section sec);

    /** \brief Updates the widget text. 
     */
    void updateText();

    quint64 m_totalSeconds; /** duration in seconds.  */

    const quint64 MIN_SECONDS = 60;             // 1 minute miminum
    const quint64 MAX_SECONDS = 30 * 24 * 3600; // 30 days maximum
};

#endif