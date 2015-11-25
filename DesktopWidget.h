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
#include <QColor>

class QPaintEvent;

class DesktopWidget
: public QWidget
{
  public:
    /** \brief DesktopWidget class constructor.
     * \param[in] name alarm name.
     * \param[in] position left-top corner position of the widget.
     * \param[in] color widget's color.
     *
     */
    explicit DesktopWidget(const QString &name, const QPoint &position, const QColor &color);

    /** \brief DesktopWidget class virtual destructor.
     *
     */
    virtual ~DesktopWidget();

    /** \brief Sets the progress of the widget.
     * \param[in] value progress value in [0.0-100.0].
     *
     */
    void setProgress(double value);

    /** \brief Enables/disables the ability to draw the widget with the left mouse button.
     * \param[in] value true to enable and false otherwise.
     *
     */
    void enableDragging(bool value);

  protected:
    virtual void mousePressEvent(QMouseEvent *e) override final;
    virtual void mouseReleaseEvent(QMouseEvent *e) override final;
    virtual void mouseMoveEvent(QMouseEvent *e) override final;

  private:
    /** \brief Draws the progress of the timer in the widget.
     *
     */
    virtual void paintEvent(QPaintEvent *e) override;

    double  m_progress;      /** progress of the widget in [0.0-100.0]. */
    QColor  m_color;         /** color of the widget. */
    QColor  m_contrastColor; /** contrast color in relation to m_color, always black or white. */
    QString m_name;          /** alarm name */
    bool    m_dragEnabled;   /** true if widget dragging is allowed and false otherwise. */

    bool   m_buttonDown; /** true if the left mouse button is down and false otherwise. */
    QPoint m_point;      /** dragging point. */
};

#endif // DESKTOPWIDGET_H_
