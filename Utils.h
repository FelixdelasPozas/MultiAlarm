/*
 File: Utils.h
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

#ifndef UTILS_H_
#define UTILS_H_

// Qt
#include <QLabel>

namespace Utils
{
  /** \class ClickableHoverLabel
  * \brief ClickableLabel subclass that changes the mouse cursor when hovered.
  *
  */
  class ClickableHoverLabel
  : public QLabel
  {
      Q_OBJECT
    public:
      /** \brief ClickableHoverLabel class constructor.
      * \param[in] parent Raw pointer of the widget parent of this one.
      * \f Widget flags.
      *
      */
      explicit ClickableHoverLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

      /** \brief ClickableHoverLabel class constructor.
      * \param[in] text Label text.
      * \param[in] parent Raw pointer of the widget parent of this one.
      * \f Widget flags.
      *
      */
      explicit ClickableHoverLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
      
      /** \brief ClickableHoverLabel class virtual destructor.
      *
      */
      virtual ~ClickableHoverLabel();

    signals:
      void clicked();

    protected:
      void mousePressEvent(QMouseEvent* e) override;

      virtual void enterEvent(QEvent *event) override;

      virtual void leaveEvent(QEvent *event) override;
  };
}

#endif // UTILS_H_