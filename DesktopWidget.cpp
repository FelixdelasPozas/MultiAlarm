/*
 File: DesktopWidget.cpp
 Created on: 25/11/2015
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
#include "DesktopWidget.h"

// Qt
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

const int DesktopWidget::WIDGET_SIZE = 100;

//-----------------------------------------------------------------
DesktopWidget::DesktopWidget(bool dragEnable, QWidget *parent)
: QWidget      {parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::NoDropShadowWindowHint|Qt::WindowTransparentForInput|Qt::Tool}
, m_progress   {0}
, m_color      {Qt::black}
, m_name       {""}
, m_buttonDown {false}
{
  // NOTE 1: attribute Qt::WA_TransparentForMouseEvents is useless, use Qt::WindowTransparentForInput instead.
  // NOTE 2: Qt::WindowTransparentForInput collides with WindowOkButtonHint so once set/unset there is no
  //         way to enable it again, thus, enabled or disabled on constructor.
  setAttribute(Qt::WA_AlwaysStackOnTop);
  setAttribute(Qt::WA_TranslucentBackground);

  if(dragEnable)
  {
    setWindowFlags(windowFlags() & ~Qt::WindowTransparentForInput);
  }

  auto desktopRect = QApplication::desktop()->geometry();
  m_limitX = desktopRect.width()-WIDGET_SIZE;
  m_limitY = desktopRect.height()-WIDGET_SIZE;

  setGeometry(QRect(0,0, WIDGET_SIZE, WIDGET_SIZE));
  setWindowOpacity(0.60);

  move(0,0);
}

//-----------------------------------------------------------------
void DesktopWidget::setProgress(double value)
{
  if((m_progress != value) && (value >= 0) && (value <= 100))
  {
    m_progress = value;

    if(isVisible())
    {
      setAttribute(Qt::WA_AlwaysStackOnTop);
      update();
      repaint();
    }
  }
}

//-----------------------------------------------------------------
void DesktopWidget::mousePressEvent(QMouseEvent* e)
{
  if(underMouse() && e->button() == Qt::MouseButton::LeftButton)
  {
    m_point = e->globalPos();

    m_buttonDown = true;
    emit beingDragged();
  }
  else
  {
    QWidget::mousePressEvent(e);
  }
}

//-----------------------------------------------------------------
void DesktopWidget::mouseReleaseEvent(QMouseEvent* e)
{
  if(underMouse() && e->button() == Qt::MouseButton::LeftButton)
  {
    m_point -= e->globalPos();
    setPosition(pos()-m_point);
    m_point = e->globalPos();

    m_buttonDown = false;
  }
  else
  {
    QWidget::mousePressEvent(e);
  }
}

//-----------------------------------------------------------------
void DesktopWidget::mouseMoveEvent(QMouseEvent* e)
{
  if(m_buttonDown)
  {
    m_point -= e->globalPos();
    setPosition(pos()-m_point);
    m_point = e->globalPos();
  }
  else
  {
    QWidget::mousePressEvent(e);
  }
}

//-----------------------------------------------------------------
void DesktopWidget::setPosition(const QPoint& position)
{
  if(position != pos())
  {
    auto newPosition = position;
    if(newPosition.x() < 0) newPosition.setX(0);
    if(newPosition.y() < 0) newPosition.setY(0);

    if(newPosition.x() > m_limitX) newPosition.setX(m_limitX);
    if(newPosition.y() > m_limitY) newPosition.setY(m_limitY);

    move(newPosition);
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
  painter.setRenderHint(QPainter::Antialiasing);

  painter.setBrush(brush);
  painter.setPen(m_contrastColor);
  painter.drawRoundRect(windowRect,40,40);

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
