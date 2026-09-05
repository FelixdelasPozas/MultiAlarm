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

// Project
#include "DurationSpinBox.h"

// Qt
#include <QLineEdit>
#include <QKeyEvent>
#include <QRegularExpression>

// C++
#include <algorithm>

//-----------------------------------------------------------------------------
DurationSpinBox::DurationSpinBox(QWidget* parent) :
    QAbstractSpinBox(parent),
    m_totalSeconds(60)
{
    lineEdit()->setAlignment(Qt::AlignLeft);
    updateText();
}

//-----------------------------------------------------------------------------
DurationSpinBox::Section DurationSpinBox::currentSection() const
{
    int pos = lineEdit()->cursorPosition();
    if (pos >= 0 && pos <= 2) {
        return Section::Days;
    }
    if (pos >= 3 && pos <= 5) {
        return Section::Hours;
    }
    if (pos >= 6 && pos <= 8) {
        return Section::Minutes;
    }
    if (pos >= 9 && pos <= 11) {
        return Section::Seconds;
    }
    return Section::None;
}

//-----------------------------------------------------------------------------
void DurationSpinBox::selectSection(Section sec)
{
    switch (sec) {
        case Section::Days:
            lineEdit()->setSelection(0, 2);
            break;
        case Section::Hours:
            lineEdit()->setSelection(3, 2);
            break;
        case Section::Minutes:
            lineEdit()->setSelection(6, 2);
            break;
        case Section::Seconds:
            lineEdit()->setSelection(9, 2);
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
void DurationSpinBox::stepBy(int steps)
{
    Section sec = currentSection();
    int64_t diff = 0; // can be negative

    switch (sec) {
        case Section::Days:
            diff = steps * 24 * 3600;
            break;
        case Section::Hours:
            diff = steps * 3600;
            break;
        case Section::Minutes:
            diff = steps * 60;
            break;
        case Section::Seconds:
            diff = steps;
            break;
        default:
            return;
    }

    // compute the new value controlling the overflowing
    quint64 oldSeconds = m_totalSeconds;
    if (diff < 0 && (quint64)std::abs(diff) > m_totalSeconds) {
        m_totalSeconds = MIN_SECONDS;
    } else {
        m_totalSeconds = std::clamp(m_totalSeconds + diff, MIN_SECONDS, MAX_SECONDS);
    }

    updateText();
    selectSection(sec);

    if (oldSeconds != m_totalSeconds) {
        emit durationChanged(m_totalSeconds);
    }
}

//-----------------------------------------------------------------------------
QAbstractSpinBox::StepEnabled DurationSpinBox::stepEnabled() const
{
    StepEnabled flags = StepNone;
    if (m_totalSeconds > MIN_SECONDS) {
        flags |= StepDownEnabled;
    }
    if (m_totalSeconds < MAX_SECONDS) {
        flags |= StepUpEnabled;
    }
    return flags;
}

//-----------------------------------------------------------------------------
QValidator::State DurationSpinBox::validate(QString& input, int& pos) const
{
    // Accept only the struct XX:XX:XX:XX with digits.
    static QRegularExpression regExp("^\\d{2}:\\d{2}:\\d{2}:\\d{2}$");

    if (regExp.match(input).hasMatch()) {
        return QValidator::Acceptable;
    }
    return QValidator::Invalid;
}

//-----------------------------------------------------------------------------
void DurationSpinBox::fixup(QString& input) const
{
    Q_UNUSED(input);
    const_cast<DurationSpinBox*>(this)->updateText();
}

//-----------------------------------------------------------------------------
void DurationSpinBox::updateText()
{
    int64_t secs = m_totalSeconds;

    int d = secs / (24 * 3600);
    secs %= (24 * 3600);
    int h = secs / 3600;
    secs %= 3600;
    int m = secs / 60;
    int s = secs % 60;

    QString text = QString("%1:%2:%3:%4")
                       .arg(d, 2, 10, QChar('0'))
                       .arg(h, 2, 10, QChar('0'))
                       .arg(m, 2, 10, QChar('0'))
                       .arg(s, 2, 10, QChar('0'));

    lineEdit()->setText(text);
}

//-----------------------------------------------------------------------------
void DurationSpinBox::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        auto sec = currentSection();
        if(event->key() == Qt::Key_Left)
        {
            if(sec != Section::Days)
            {
                auto secPos = static_cast<int>(sec);
                selectSection(static_cast<Section>(--secPos));
            }
        }

        if(event->key() == Qt::Key_Right)
        {
            if(sec != Section::Seconds)
            {
                auto secPos = static_cast<int>(sec);
                selectSection(static_cast<Section>(++secPos));
            }
        }
        return;
    }

    // If you try to write numbers directly, we process the logic in blocks of 2 digits
    if (event->text().contains(QRegularExpression("^\\d$"))) {
        int pos = lineEdit()->cursorPosition();
        QString currentText = lineEdit()->text();

        // We overwrite the character under the cursor instead of pushing the formatting to the right
        if (pos < currentText.length() && currentText[pos] != ':') {
            currentText[pos] = event->text()[0];

            // We evaluate if the new temporary string is transformed into valid seconds
            QStringList parts = currentText.split(':');
            if (parts.size() == 4) {
                quint64 newSecs = parts[0].toULongLong() * 24 * 3600 + parts[1].toULongLong() * 3600 +
                                  parts[2].toULongLong() * 60 + parts[3].toULongLong();

                if (parts[0].toUInt() <= 10 && parts[1].toUInt() < 24 && parts[2].toUInt() < 60 &&
                    parts[3].toUInt() < 60) {
                    quint64 oldSeconds = m_totalSeconds;
                    m_totalSeconds = std::clamp(newSecs, MIN_SECONDS, MAX_SECONDS);

                    if (oldSeconds != m_totalSeconds) {
                        emit durationChanged(m_totalSeconds);
                    }
                }
            }
            updateText();

            // Move the cursor smoothly
            int nextPos = pos + 1;
            if (nextPos < currentText.length() && lineEdit()->text()[nextPos] == ':') {
                nextPos++; // Skip the separator ':'
            }
            lineEdit()->setCursorPosition(nextPos);
            selectSection(currentSection());
            return;
        }
    }

    QAbstractSpinBox::keyPressEvent(event);
}

//-----------------------------------------------------------------------------
void DurationSpinBox::mousePressEvent(QMouseEvent* event)
{
    QAbstractSpinBox::mousePressEvent(event);
    selectSection(currentSection());
}

//-----------------------------------------------------------------------------
void DurationSpinBox::setDurationInSeconds(quint64 seconds)
{
    quint64 validatedSeconds = std::clamp(seconds, MIN_SECONDS, MAX_SECONDS);
    if (m_totalSeconds != validatedSeconds) {
        m_totalSeconds = validatedSeconds;
        updateText();
        emit durationChanged(m_totalSeconds);
    }
}