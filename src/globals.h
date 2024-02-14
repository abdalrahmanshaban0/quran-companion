/**
 * @file globals.h
 * @brief Header file defining application-wide variables.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QtAwesome.h>

/**
 * @struct Reciter
 * @brief Reciter struct represents a quran reciter
 */
struct Reciter
{
  QString baseDirName{}; ///< The name of the directory conatining recitations
                         ///< in the application recitations directory.
  QString displayName{}; ///< The reciter name as its displayed in the UI.
  QString
    basmallahPath{}; ///< Absolute path to the reciters bismillah audio file.
  QString baseUrl{}; ///< Url to download recitation files from.
  bool useId{
    false
  }; ///< Boolean value representing whether the verse recitations should be
     ///< downloading using the verse number relative to the beginning of the
     ///< Quran or a combination of surah and verse numbers.
};
/**
 * @brief Tafsir struct contains data about a single tafsir
 * @details tafasir are stored in the resource file "files.xml"
 */
struct Tafsir
{
  QString displayName;
  QString filename;
  bool text;
  bool extra;
};
/**
 * @brief Translation struct holds different values representing available Quran
 * translations
 */
struct Translation
{
  QString displayName;
  QString filename;
  bool extra;
};

enum DownloadType
{
  QCF,
  Recitation,
  File
};

enum VerseType
{
  qcf,
  uthmanic,
  annotated
};

/**
 * @brief ReaderMode enum represents the available modes for the Quran reader in
 * MainWindow
 */
enum ReaderMode
{
  SinglePage, ///< Single Quran page, side panel is used for displaying verses
              ///< with translation
  DoublePage  ///< Two Quran pages, both panels are used to display Quran pages,
              ///< no translation
};

namespace Globals {
extern int
  themeId; ///< global variable represnting the application theme index.

extern QSettings*
  settings; ///< global pointer to the application QSettings instance.

extern bool
  darkMode; ///< global boolean to indicate if application is in dark mode.

extern QLocale::Language
  language; ///< global QLocale::Language instance for application languge.

extern QString
  updateToolPath; ///< global absolute path for the application update tool.

extern int qcfVersion; ///< global variable for the QCF version in use.

extern ReaderMode readerMode;

extern QString qcfFontPrefix; ///< global variable for the QCF font prefix to
                              ///< generate font name from.

extern QDir themeResources; ///< global QDir for the current theme resources
                            ///< (icons & styles).
extern QDir
  configDir; ///< global QDir representing application config directiory.

extern QDir assetsDir; ///< global QDir representing the application assets
                       ///< directory (fonts, translations, tafsir).

extern QDir
  bismillahDir; ///< global QDir representing the reciters basmallah files.

extern QDir downloadsDir; ///< global QDir representing the top-level path
                          ///< for downloaded files.

extern QList<Reciter> recitersList; ///< global QList containing reciters
                                    ///< supported by the application.

extern QList<Tafsir> tafasirList;

extern QList<Translation> translationsList;

extern QMap<QString, QString>
  shortcutDescription; ///< global QMap containing all available
                       ///< application shortcuts as keys and their descriptions
                       ///< as values.

extern fa::QtAwesome*
  awesome; ///< global pointer used for generating font awesome icons

extern QString
pageFontname(int page);
extern QString
verseFontname(VerseType type, int page);
extern bool
tafsirExists(const Tafsir* tafsir);
extern bool
translationExists(const Translation* tr);
};

#endif // GLOBALS_H
