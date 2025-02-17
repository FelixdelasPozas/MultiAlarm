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
#include <QFrame>
#include <QColor>

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

      virtual void enterEvent(QEnterEvent *event) override;

      virtual void leaveEvent(QEvent *event) override;
  };
}

/** \class DrawFrame
 * \brief A QFrame that draw a progress gradient in its background.
 *
 */
class DrawFrame
: public QFrame
{
    Q_OBJECT
  public:
    /** \brief DrawFrame class constructor. 
     * \param[in] parent Raw pointer of the widget parent of this one. 
     * \param[in] f Widget flags.
     *
     */
    explicit DrawFrame(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
    : QFrame(parent, f)
    {};

    /** \brief DrawFrame class virtual destructor.
     *
     */
    virtual ~DrawFrame()
    {};

    /** \brief Sets the progress value in [0,1]. 
     * \param[in] p Double value in [0,1].
     *
     */
    void setProgress(const qreal p)
    { m_progress = std::min(1., std::max(0., p)); }
    
    /** \brief Sets the gradient colors.
     * \param[in] a First gradient color. 
     * \param[in] b Second gradient color. 
     *
     */
    void setColors(const QColor &a, const QColor &b)
    { m_color = a; m_shineColor = b; }

  protected:
    void paintEvent(QPaintEvent *p) override;

  private:
    qreal m_progress = 0;                        /** progress value for drawing the gradients. */
    QColor m_color = QColor(0, 0, 0);            /** first gradient color. */
    QColor m_shineColor = QColor(255, 255, 255); /** second gradient color. */
};

#endif // UTILS_H_