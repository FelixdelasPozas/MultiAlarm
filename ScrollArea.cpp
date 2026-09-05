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
    QScrollArea{parent}
{
    setAcceptDrops(true);

    setLayout(new QVBoxLayout(this));
    layout()->setSpacing(1);
    layout()->setContentsMargins(0,0,0,0);
}

//-----------------------------------------------------------------------------
void ScrollArea::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

//-----------------------------------------------------------------------------
void ScrollArea::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

//-----------------------------------------------------------------------------
void ScrollArea::dropEvent(QDropEvent* event)
{
    event->acceptProposedAction();
}

//-----------------------------------------------------------------------------
void ScrollArea::dropped(QWidget* from, QWidget* to)
{
    int fromIndex = -1, toIndex = -1;
    auto vlayout = qobject_cast<QVBoxLayout*>(layout());
    for (int i = 0; i < vlayout->count(); ++i) {
        QWidget* widget = vlayout->itemAt(i)->widget();
        if (widget) {
            if (widget == from) {
                fromIndex = i;
                continue;
            }

            if (widget == to) {
                toIndex = i;
            }
        }
    }

    if(fromIndex == -1 || toIndex == -1)
        return;
        
    vlayout->removeWidget(from);
    vlayout->insertWidget(toIndex, from);
}
