/*
 File: DesktopWidget.cpp
 Created on: 25 de nov. de 2015
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

// project
#include <DesktopWidget.h>

// Qt
#include <QPainter>
#include <QLabel>
#include <QPaintEngine>
#include <QMouseEvent>

//-----------------------------------------------------------------
DesktopWidget::DesktopWidget(const QString &name, const QPoint &position, const QColor &color)
: QWidget      {nullptr}
, m_progress   {0}
, m_color      {color}
, m_name       {name}
, m_dragEnabled{false}
, m_buttonDown {false}
{
  // NOTE: attribute Qt::WA_TransparentForMouseEvents is useless, use Qt::WindowTransparentForInput instead.
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_AlwaysStackOnTop);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::WindowTransparentForInput|Qt::Tool);

  setGeometry(QRect(0,0, 100, 100));
  move(position);
  setWindowOpacity(0.60);

  auto blackDistance = color.red() + color.green() + color.blue();
  auto whiteDistance = (254 * 3) - blackDistance;

  m_contrastColor = (blackDistance < whiteDistance ? "white" : "black");

  hide();
}

//-----------------------------------------------------------------
DesktopWidget::~DesktopWidget()
{
}

//-----------------------------------------------------------------
void DesktopWidget::setProgress(double value)
{
  if((m_progress != value) && (value >= 0) && (value <= 100))
  {
    m_progress = value;

    this->repaint();
  }
}

//-----------------------------------------------------------------
void DesktopWidget::enableDragging(bool value)
{
  if(value != m_dragEnabled)
  {
    m_dragEnabled = value;
    Qt::WindowFlags flags = windowFlags();

    if(value)
    {
      flags &= ~Qt::WindowTransparentForInput;
    }
    else
    {
      flags |= Qt::WindowTransparentForInput;
    }

    setWindowFlags(flags);
  }
}

//-----------------------------------------------------------------
void DesktopWidget::mousePressEvent(QMouseEvent* e)
{
  if(m_dragEnabled && (e->button() == Qt::MouseButton::LeftButton))
  {
    m_point = e->globalPos();

    m_buttonDown = true;
  }
}

//-----------------------------------------------------------------
void DesktopWidget::mouseReleaseEvent(QMouseEvent* e)
{
  if(m_dragEnabled && (e->button() == Qt::MouseButton::LeftButton))
  {
    m_point -= e->globalPos();
    move(pos()-m_point);
    m_point = e->globalPos();

    m_buttonDown = false;
  }
}

//-----------------------------------------------------------------
void DesktopWidget::mouseMoveEvent(QMouseEvent* e)
{
  if(m_dragEnabled && m_buttonDown)
  {
    m_point -= e->globalPos();
    move(pos()-m_point);
    m_point = e->globalPos();
  }
}

//-----------------------------------------------------------------
void DesktopWidget::paintEvent(QPaintEvent *e)
{
  QBrush brush(m_contrastColor, Qt::SolidPattern);
  auto windowRect = rect();

  QPainter painter;
  painter.begin(this);

  painter.setBrush(brush);
  painter.drawRoundRect(windowRect);

  brush.setColor(m_color);
  brush.setStyle(Qt::SolidPattern);
  painter.setPen(m_color);
  painter.setBrush(brush);

  auto smallRect = QRect{windowRect.x()+5, windowRect.y()+5, windowRect.width()-10, windowRect.height()-10};
  auto progressValue = (360.0 - 360.0*(m_progress/100.0))*16;
  painter.drawPie(smallRect, 90*16, progressValue);

  QString displayText;
  auto parts = m_name.split(" ");
  for(auto part: parts)
  {
    displayText.append(part + (part != parts.last() ? "\n": ""));
  }

  auto color = QColor{(m_contrastColor.red()+m_color.red())/2,
                      (m_contrastColor.green()+m_color.green())/2,
                      (m_contrastColor.blue()+m_color.blue())/2};
  smallRect = QRect{windowRect.x()+2, windowRect.y()+2, windowRect.width()-2, windowRect.height()-2};

  painter.setFont(QFont("Arial", 10));
  painter.setPen(color);
  painter.drawText(smallRect, Qt::AlignCenter, displayText);

  painter.setPen((m_contrastColor == Qt::white ? Qt::black : Qt::white));
  painter.drawText(windowRect, Qt::AlignCenter, displayText);
  painter.end();
}
