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

const int DesktopWidget::WIDGET_SIZE = 100;

//-----------------------------------------------------------------
DesktopWidget::DesktopWidget()
: QWidget      {nullptr}
, m_progress   {0}
, m_color      {Qt::black}
, m_name       {""}
, m_dragEnabled{false}
, m_buttonDown {false}
{
  // NOTE 1: attribute Qt::WA_TransparentForMouseEvents is useless, use Qt::WindowTransparentForInput instead.
  // NOTE 2: Qt::WindowTransparentForInput collides with WindowOkButtonHint so once set/unset there is no
  //         way to enable it again.
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_AlwaysStackOnTop);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::WindowTransparentForInput|Qt::Tool);
  setGeometry(QRect(0,0, WIDGET_SIZE, WIDGET_SIZE));
  setWindowOpacity(0.60);

  move(0,0);

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

    if(isVisible()) repaint();
  }
}

//-----------------------------------------------------------------
void DesktopWidget::enableDragging(bool value)
{
  return; // Disabled for now, see notes in constructor.

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
void DesktopWidget::setPosition(const QPoint& position)
{
  if(position != pos())
  {
    move(position);
  }
}

//-----------------------------------------------------------------
void DesktopWidget::setOpacity(const int opacity)
{
  if(opacity != windowOpacity())
  {
    setWindowOpacity(opacity/100.0);

    if(isVisible()) repaint();
  }
}

//-----------------------------------------------------------------
void DesktopWidget::setColor(const QColor& color)
{
  m_color = color;

  auto blackDistance = color.red() + color.green() + color.blue();
  auto whiteDistance = (254 * 3) - blackDistance;

  m_contrastColor = (blackDistance < whiteDistance ? "white" : "black");

  if(isVisible()) repaint();
}

//-----------------------------------------------------------------
void DesktopWidget::setName(const QString &name)
{
  if(m_name != name)
  {
    m_name = name;

    if(isVisible()) repaint();
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

  auto color = (m_contrastColor == Qt::black ? Qt::white : Qt::black);
  smallRect = QRect{windowRect.x()+2, windowRect.y()+2, windowRect.width()-2, windowRect.height()-2};

  painter.setFont(QFont("Arial", 10));
  painter.setPen(color);
  painter.drawText(smallRect, Qt::AlignCenter, displayText);

  painter.setPen(m_contrastColor);
  painter.drawText(windowRect, Qt::AlignCenter, displayText);
  painter.end();
}
