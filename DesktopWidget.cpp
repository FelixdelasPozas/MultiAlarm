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

//-----------------------------------------------------------------
DesktopWidget::DesktopWidget(QWidget *parent)
: QWidget{parent}
{
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAttribute(Qt::WA_AlwaysStackOnTop);
  setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
  setGeometry(QRect(250,250, 500, 500));
  setWindowOpacity(0.50);

//  m_background = new QLabel(this);
//  m_background->setFixedSize(size());
//  m_background->setStyleSheet("background: black; border-radius: 20px;");
}

//-----------------------------------------------------------------
DesktopWidget::~DesktopWidget()
{
//  delete m_background;
//  m_background = nullptr;
}

//-----------------------------------------------------------------
void DesktopWidget::paintEvent(QPaintEvent *e)
{
  QBrush brush(Qt::blue, Qt::SolidPattern);

  QPainter painter;
  painter.begin(this);
  painter.setBrush(brush);
  painter.drawPie(rect(), 90*16, 359*16);
  painter.end();
}
