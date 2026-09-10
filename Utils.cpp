/*
 File: Utils.cpp
 Created on: 13/01/2015
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
#include <Utils.h>

// Qt
#include <QPainter>
#include <QPainterPath>
#include <QApplication>

//-----------------------------------------------------------------
Utils::ClickableHoverLabel::ClickableHoverLabel(QWidget* parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
}

//-----------------------------------------------------------------
Utils::ClickableHoverLabel::ClickableHoverLabel(const QString& text, QWidget* parent, Qt::WindowFlags f) :
    QLabel(text, parent, f)
{
}

//-----------------------------------------------------------------
Utils::ClickableHoverLabel::~ClickableHoverLabel()
{
}

//-----------------------------------------------------------------
void Utils::ClickableHoverLabel::mousePressEvent(QMouseEvent* e)
{
    emit clicked();
    QLabel::mousePressEvent(e);
}

//-----------------------------------------------------------------
void Utils::ClickableHoverLabel::enterEvent(QEnterEvent* event)
{
    setCursor(Qt::PointingHandCursor);
    QLabel::enterEvent(event);
}

//-----------------------------------------------------------------
void Utils::ClickableHoverLabel::leaveEvent(QEvent* event)
{
    setCursor(Qt::ArrowCursor);
    QLabel::leaveEvent(event);
}

//-----------------------------------------------------------------
void DrawFrame::paintEvent(QPaintEvent* p)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);

    auto makeStops = [](const QColor& a, const QColor& b) {
        QGradientStops stops;
        stops << QGradientStop{0, a} << QGradientStop{0.4, b} << QGradientStop{0.6, b} << QGradientStop{1, a};
        return stops;
    };

    auto rect = this->rect();
    auto width = rect.width();
    const int progressWidth = std::max(0, std::min(static_cast<int>(std::nearbyint(m_progress * width)), width));

    QLinearGradient uncompletedGradient;
    uncompletedGradient.setCoordinateMode(QGradient::ObjectMode);
    uncompletedGradient.setStart({0, 0});
    uncompletedGradient.setFinalStop({0, 1});
    uncompletedGradient.setStops(makeStops(m_color, m_shineColor));

    QPainterPath uncompletedPath;
    uncompletedPath.addRoundedRect(rect, 5, 5);
    painter.fillPath(uncompletedPath, uncompletedGradient);

    QLinearGradient completedGradient;
    completedGradient.setCoordinateMode(QGradient::ObjectMode);
    completedGradient.setStart({0, 0});
    completedGradient.setFinalStop({0, 1});
    completedGradient.setStops(makeStops(m_shineColor, m_color.darker()));

    rect.setWidth(progressWidth);
    QPainterPath completedPath;
    completedPath.addRoundedRect(rect, 5, 5);
    painter.fillPath(completedPath, completedGradient);

    painter.drawRoundedRect(this->rect(), 5, 5);

    painter.end();

    QFrame::paintEvent(p);
}

//----------------------------------------------------------------------------
void AutoCloseMessageBox::showEvent(QShowEvent* event)
{
    QMessageBox::showEvent(event);
    QApplication::beep();
    m_text = text();

    m_currentTime = 0;
    if (m_autoClose) {
        this->startTimer(1000); // counting is done in 'timerEvent'.
    }
}

//----------------------------------------------------------------------------
void AutoCloseMessageBox::timerEvent(QTimerEvent* event)
{
    m_currentTime++; // counting.

    if (m_autoClose) {
        setText(m_text + QString("\nThe alarm will stop in %1 seconds.").arg(m_closeSeconds - m_currentTime));
        if (m_currentTime >= m_closeSeconds) {
            this->done(0);
        }
    }
}

//----------------------------------------------------------------------------
void AutoCloseMessageBox::setAutoClose(const bool value)
{
    m_autoClose = value;
};

//----------------------------------------------------------------------------
void AutoCloseMessageBox::setCloseTime(const unsigned int seconds)
{
    m_autoClose = seconds >= 5;
    m_closeSeconds = seconds;
}

//----------------------------------------------------------------------------
bool ReadRegistryDword(HKEY root, const std::wstring& subKey, const std::wstring& valueName, DWORD& outValue)
{
    HKEY hKey;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    DWORD dataSize = sizeof(DWORD);
    LONG result =
        RegQueryValueExW(hKey, valueName.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(&outValue), &dataSize);
    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS);
}

//----------------------------------------------------------------------------
void centerDialog(QDialog* dialog, const unsigned int minimumWidth)
{
    if (dialog) {
        dialog->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        dialog->setMinimumSize(minimumWidth, 0);

        dialog->adjustSize();
        dialog->setFixedSize(dialog->size());

        auto rect = QApplication::activeWindow()->geometry();
        dialog->move(rect.center() - dialog->rect().center());
    }
}

//----------------------------------------------------------------------------
void showMessageBox(const QMessageBox::Icon& icon, const QString& msg, const QString& details, const QString& title)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    msgBox.setIcon(icon);
    if(!details.isEmpty()) msgBox.setDetailedText(details);
    msgBox.setWindowIcon(QIcon(":/MultiAlarm/application.svg"));
    msgBox.exec();  
}
