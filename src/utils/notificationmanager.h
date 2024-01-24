/**
 * @file notificationmanager.h
 * @brief Header file for NotificationManager
 */

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "../globals.h"
#include "dbmanager.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

/**
 * @brief NotificationManager class is responsible for system tray functionality
 * and verse of the day notification
 */
class NotificationManager : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit NotificationManager(QObject* parent = nullptr);
  ~NotificationManager();

  /**
   * @brief send a desktop notification message
   * @param title - notification title
   * @param msg - notification message
   */
  void notify(QString title, QString msg);
  /**
   * @brief sets the tooltip for the system tray icon
   * @param text - tooltip message
   */
  void setTooltip(QString text);

signals:
  /**
   * @fn void exit(int)
   * @brief signal connected to the 'Exit' systray action
   * @param ret - exit code
   */
  void exit(int ret = 0);
  /**
   * @fn void togglePlayback()
   * @brief signal connected to 'toggle play/pause' systray action
   */
  void togglePlayback();
  /**
   * @fn void checkForUpdates()
   * @brief signal connected to 'check for updates' systray action
   */
  void checkForUpdates();
  /**
   * @fn void showWindow()
   * @brief signal connected to 'show window' systray action
   */
  void showWindow();
  /**
   * @fn void hideWindow()
   * @brief signal connected to 'hide window' systray action
   */
  void hideWindow();
  /**
   * @fn void openPrefs()
   * @brief signal connected to 'preferences' systray action
   */
  void openPrefs();
  /**
   * @fn void openAbout()
   * @brief signal connected to 'about' systray action
   */
  void openAbout();

private:
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  /**
   * @brief adds system tray actions and set their connections
   */
  void addActions();
  /**
   * @brief system tray context menu
   */
  QMenu* m_trayMenu;
  /**
   * @brief QSystemTrayIcon instance
   */
  QSystemTrayIcon* m_sysTray;
};

#endif // NOTIFICATIONMANAGER_H
