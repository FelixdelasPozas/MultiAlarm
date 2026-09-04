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
#include <QMessageBox>
#include <QTimer>

// C++
#include <windows.h>

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

/** \brief Implementation of an autoclose QMessageBox.
 */
class AutoCloseMessageBox : public QMessageBox
{
    Q_OBJECT
  public:
    /**
     * \brief AutoCloseMessageBox class constructor. 
     * \param[in] parent Raw pointer of the widget parent of this one.
     *
     */
    AutoCloseMessageBox(QWidget* parent = nullptr) :
        QMessageBox{parent} {};

    /** \brief AutoCloseMessageBox class constructor. 
     * \param[in] icon Message box icon.
     * \param[in] title Dialog title.
     * \param[in] text Dialog text.
     * \param[in] buttons Buttons to show. 
     * \param[in] parent Raw pointer of the widget parent of this one. 
     * \param[in] flags Dialog flags. 
     */
    AutoCloseMessageBox(Icon icon, const QString& title, const QString& text, StandardButtons buttons = NoButton,
                        QWidget* parent = Q_NULLPTR,
                        Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) :
        QMessageBox(icon, title, text, buttons, parent, flags) {};

    /** \brief AutoCloseMessageBox class virtual destructor. 
     */
    virtual ~AutoCloseMessageBox() = default;

    /** \brief Sets if the dialog must auto-close.
     * \param[in] value True to auto-close and false otherwise. 
     */
    void setAutoClose(const bool value);

    /** \brief Sets the closing time.
     * \param[in] seconds Closing time in seconds. 
     */
    void setCloseTime(const unsigned int seconds);

  protected:
    void showEvent(QShowEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

  private:
    QString m_text;                 /** text to show. */
    uint32_t m_closeSeconds = 5;    /** seconds to close. */
    bool m_autoClose = false;        /** true to auto-close false to act as a regular QMessageBox. */
    unsigned int m_currentTime = 0; /** current time since showing the messagebox. */
    int m_timerId = 0;              /** current timer id. */
};

/** \brief Reads a registry word. Returns true on success.
 * \param[in] root Registry root
 * \param[in] subKey Subkey value
 * \param[in] valueName Name of registry key.
 * \param[out] outValue Key value. 
 */
bool ReadRegistryDword(HKEY root, const std::wstring& subKey, const std::wstring& valueName, DWORD& outValue);

#endif // UTILS_H_