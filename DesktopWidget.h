/*
 File: DesktopWidget.h
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

#ifndef DESKTOPWIDGET_H_
#define DESKTOPWIDGET_H_

// Qt
#include <QWidget>

class QPaintEvent;
class QLabel;

class DesktopWidget
: public QWidget
{
  public:
    /** \brief DesktopWidget class constructor.
     *
     */
    explicit DesktopWidget(QWidget *parent = nullptr);

    /** \brief DesktopWidget class virtual destructor.
     *
     */
    virtual ~DesktopWidget();

  private:
    /** \brief Draws the progress of the timer in the widget.
     *
     */
    virtual void paintEvent(QPaintEvent *e) override;

    QLabel *m_background;
};

#endif // DESKTOPWIDGET_H_
