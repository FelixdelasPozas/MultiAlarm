/*
 File: LogiLED.h
 Created on: 21/11/2018
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

#ifndef LOGILED_H_
#define LOGILED_H_

// C++
#include <memory>

// Qt
#include <QObject>
#include <QString>
#include <QColor>
#include <QReadWriteLock>

/** \class LogiLED
 * \brief Interface to Logitech Gaming LED SDK
 *
 */
class LogiLED
: public QObject
{
    Q_OBJECT
  public:
    /** \brief LogiLED class virtual destructor.
     *
     */
    virtual ~LogiLED();

    /** \brief Gets the LogiLED singleton instance.
     *
     */
    static LogiLED & getInstance();

    /** \brief Deleted copy constructor to avoid copying the singleton.
     *
     */
    LogiLED(LogiLED const&) = delete;

    /** \brief Deleted operator= to avoid copying the singleton.
     *
     */
    void operator=(LogiLED const&) = delete;

    /** \brief Returns true if the interface to keyboard lights is available and false otherwise.
     *
     */
    static bool isAvailable();

    /** \brief Registers a new item. Returns true on succes and false if there is already an item with the same id.
     * \param[in] id Item identifier.
     * \param[in] progress Progress value in [0,100].
     * \param[in] foreground Foreground color.
     * \param[in] background Background color.
     *
     */
    bool registerItem(const QString &id, const int progress, const QColor &foreground, const QColor &background);

    /** \brief Unregisters an item. Returns true on success and false if the item couldn't be found.
     * \param[in] id Item identifier.
     *
     */
    bool unregisterItem(const QString &id);

    /** \brief Updates the internal values of an item. Returns true on success and false if the item couldn't be found.
     * \param[in] id Item identifier.
     * \param[in] progress Progress value in [0,100].
     * \param[in] foreground Foreground color. Can be empty.
     * \param[in] background Background color. Can be empty.
     *
     */
    bool updateItem(const QString &id, const int progress, const QColor &foreground = QColor(), const QColor &background = QColor());

  private slots:
    /** \brief Updates the keyboard lights.
     *
     */
    void updateLights();

  private:
    /** \brief LogilLED class private constructor.
     *
     */
    LogiLED();

    /** \brief Returns true if the interface to keyboard lights is available and false otherwise.
     *
     */
    const bool isInitialized() const
    { return m_available; }

    /** \brief Shutdowns and re-initializes the session to restore default keyboard lights.
     *
     */
    void restart() const;

    /** \brief Changes the keys to the given parameters.
     * \param[in] progress Value in [0,100].
     * \param[in] foreground Foreground color.
     * \param[in] background Background color.
     *
     */
    void changeKeysToColor(const int progress, const QColor foreground, const QColor background);

    /** \struct Item
     * \brief Holds internal data for a registered item.
     *
     */
    struct Item
    {
      QString id;         /** item identifier.           */
      int     progress;   /** progress value in [0,100]. */
      QColor  foreground; /** background color for keys. */
      QColor  background; /** foreground color for keys. */

      /** \brief Item constructor.
       * \param[in] i Item identifier.
       * \param[in] p Progress value in [0,100]
       * \param[in] f Foreground color.
       * \param[in] b Background color.
       *
       */
      Item(const QString &i, const int p, const QColor &f, const QColor &b): id(i), progress(p), foreground(f), background(b) {};

      /** \brief Operator==
       * \param[in] other Other Item struct reference.
       *
       */
      bool operator==(const Item &other)
      {
        return (id == other.id);
      }
    };

    bool               m_available; /** true if the LogiLED API is available and initialized. */
    QList<struct Item> m_items;     /** list of registered items to show in the keyboard.     */
    int                m_current;   /** index to current item showing.                        */
    QReadWriteLock     m_lock;      /** data protection mutex.                                */
};

#endif // LOGILED_H_
