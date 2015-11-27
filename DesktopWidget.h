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

class DesktopWidget
: public QWidget
{
    Q_OBJECT
  public:
    static const int WIDGET_SIZE; /** height and width of the widget in pixels. */

    /** \brief DesktopWidget class constructor.
     * \param[in] dragEnable true to make the widget react to events and false to make the widget transparent to input.
     *
     */
    explicit DesktopWidget(bool dragEnable, QWidget *parent);

    /** \brief DesktopWidget class virtual destructor.
     *
     */
    virtual ~DesktopWidget();

    /** \brief Sets the progress of the widget.
     * \param[in] value progress value in [0.0-100.0].
     *
     */
    void setProgress(double value);

    /** \brief Sets the widget position on the screen.
     * \param[in] position position coordinates.
     *
     */
    void setPosition(const QPoint &position);

    /** \brief Modifies the widget opacity.
     * \param[in] opacity opacity value in [0-100].
     *
     */
    void setOpacity(const int opacity);

    /** \brief Sets the widget color.
     * \param[in] color widget color.
     *
     */
    void setColor(const QColor &color);

    /** \brief Sets the name of the widget.
     *
     */
    void setName(const QString &name);

  signals:
    void beingDragged();

  protected:
    virtual void mousePressEvent(QMouseEvent *e) override final;
    virtual void mouseReleaseEvent(QMouseEvent *e) override final;
    virtual void mouseMoveEvent(QMouseEvent *e) override final;

  private:
    virtual void paintEvent(QPaintEvent *e) override;

    double  m_progress;      /** progress of the widget in [0.0-100.0]. */
    QColor  m_color;         /** color of the widget. */
    QColor  m_contrastColor; /** contrast color in relation to m_color, always black or white. */
    QString m_name;          /** alarm name */

    bool   m_buttonDown;     /** true if the left mouse button is down and false otherwise. */
    QPoint m_point;          /** dragging point. */

    unsigned int m_limitX;   /** X limit in global screen coordinates. */
    unsigned int m_limitY;   /** Y limit in global screen coordinates. */
};

#endif // DESKTOPWIDGET_H_
