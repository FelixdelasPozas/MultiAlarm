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

// Project
#include "ScrollArea.h"

// Qt
#include <QDragEnterEvent>
#include <QDrag>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
ScrollArea::ScrollArea(QWidget* parent) :
    QScrollArea{parent},
    m_dragPosition{-1},
    m_dragWidget{nullptr}
{
    setAcceptDrops(true);
}

//-----------------------------------------------------------------------------
void ScrollArea::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
        const auto pos = this->mapFromGlobal(QCursor::pos()).y();
        const auto h = this->widget()->layout()->itemAt(0)->widget()->height();
        const auto dragPos = pos/h;

        auto vlayout = qobject_cast<QVBoxLayout*>(widget()->layout());
        if(dragPos > vlayout->count() - 1) return;
        if(m_dragPosition != -1 && dragPos != m_dragPosition)
        {
            vlayout->removeWidget(m_dragWidget);
            vlayout->insertWidget(dragPos, m_dragWidget);
        }
        m_dragPosition = dragPos;
        m_dragWidget = widget()->layout()->itemAt(m_dragPosition)->widget();
    }
}

//-----------------------------------------------------------------------------
void ScrollArea::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
    const auto pos = this->mapFromGlobal(QCursor::pos()).y();
    const auto h = this->widget()->layout()->itemAt(0)->widget()->height();
    const auto dragPos = pos / h;
    auto vlayout = qobject_cast<QVBoxLayout*>(widget()->layout());
    if(dragPos > vlayout->count() - 1) return;

    if (m_dragPosition != dragPos) {
        vlayout->removeWidget(m_dragWidget);
        vlayout->insertWidget(dragPos, m_dragWidget);
        m_dragPosition = dragPos;
    }
}

//-----------------------------------------------------------------------------
void ScrollArea::dropEvent(QDropEvent* event)
{
    event->acceptProposedAction();
    cancelled();
    repaint();
}

//-----------------------------------------------------------------------------
void ScrollArea::cancelled()
{
    m_dragPosition = -1;
    m_dragWidget = nullptr;
}
