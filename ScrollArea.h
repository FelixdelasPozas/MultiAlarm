/*
 File: ScrollArea.h
 Created on: 03/09/2026
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

#ifndef _SCROLL_AREA_H_
#define _SCROLL_AREA_H_

// Qt
#include <QScrollArea>

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

/** \class ScrollArea
 * \brief Implements a scroll area with draggable items. 
 */
class ScrollArea : public QScrollArea
{
    Q_OBJECT
  public:
    /** \brief ScrollArea class constructor.
     * \param[in] parent Raw pointer of the widget parent of this one. 
     */
    ScrollArea(QWidget* parent = nullptr);

    /** \brief ScrollArea class virtual destructor. 
     */
    virtual ~ScrollArea() = default;

    // NOTE: public to notify scroll area and move widgets...
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dragMoveEvent(QDragMoveEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

    /** \brief Cancels the drag event.
     */
    void cancelled();

  private:
    int m_dragPosition; /** position of the dragged item */
    QWidget *m_dragWidget; /** dragged widget. */
};

#endif