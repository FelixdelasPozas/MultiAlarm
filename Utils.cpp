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

//-----------------------------------------------------------------
Utils::ClickableHoverLabel::ClickableHoverLabel(QWidget *parent, Qt::WindowFlags f)
: QLabel(parent, f)
{}

//-----------------------------------------------------------------
Utils::ClickableHoverLabel::ClickableHoverLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
: QLabel(text, parent, f)
{}

//-----------------------------------------------------------------
Utils::ClickableHoverLabel::~ClickableHoverLabel()
{}

//-----------------------------------------------------------------
void Utils::ClickableHoverLabel::mousePressEvent(QMouseEvent *e)
{
  emit clicked();
  QLabel::mousePressEvent(e);
}

//-----------------------------------------------------------------
void Utils::ClickableHoverLabel::enterEvent(QEnterEvent *event)
{
  setCursor(Qt::PointingHandCursor);
  QLabel::enterEvent(event);
}

//-----------------------------------------------------------------
void Utils::ClickableHoverLabel::leaveEvent(QEvent *event)
{
  setCursor(Qt::ArrowCursor);
  QLabel::leaveEvent(event);
}

//-----------------------------------------------------------------
void DrawFrame::paintEvent(QPaintEvent *p)
{
  QPainter painter(this);
  painter.setPen(Qt::black);
  painter.setRenderHint(QPainter::Antialiasing);

  auto makeStops = [](const QColor &a, const QColor &b)
  {
    QGradientStops stops;
    stops << QGradientStop{0,a} << QGradientStop{0.4,b} << QGradientStop{0.6,b} << QGradientStop{1,a};
    return stops;
  };

  auto rect = this->rect();
  auto width = rect.width();
  const int progressWidth = std::max(0, std::min(static_cast<int>(std::nearbyint(m_progress * width)), width));

  QLinearGradient uncompletedGradient;
  uncompletedGradient.setCoordinateMode(QGradient::ObjectMode);
  uncompletedGradient.setStart({0,0});
  uncompletedGradient.setFinalStop({0,1});
  uncompletedGradient.setStops(makeStops(m_color, m_shineColor));

  QPainterPath uncompletedPath;
  uncompletedPath.addRoundedRect(rect, 5, 5);
  painter.fillPath(uncompletedPath, uncompletedGradient);

  QLinearGradient completedGradient;
  completedGradient.setCoordinateMode(QGradient::ObjectMode);
  completedGradient.setStart({0,0});
  completedGradient.setFinalStop({0,1});
  completedGradient.setStops(makeStops(m_shineColor, m_color.darker()));

  rect.setWidth(progressWidth);
  QPainterPath completedPath;
  completedPath.addRoundedRect(rect, 5, 5);
  painter.fillPath(completedPath, completedGradient);

  painter.drawRoundedRect(this->rect(), 5, 5);

  painter.end();

  QFrame::paintEvent(p);
}
