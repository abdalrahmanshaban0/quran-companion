/**
 * @file mainwindow.h
 * @brief Header file for MainWindow
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/notificationmanager.h"
#include "../utils/verseplayer.h"
#include "../widgets/notificationpopup.h"
#include "../widgets/quranpagebrowser.h"
#include "../widgets/verseframe.h"
#include "bookmarksdialog.h"
#include "downloaderdialog.h"
#include "searchdialog.h"
#include "settingsdialog.h"
#include "tafsirdialog.h"
#include <QBoxLayout>
#include <QClipboard>
#include <QDesktopServices>
#include <QIntValidator>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief MainWindow class is responsible for the reader interface, audio
 * controls, navigation, and opening other dialogs
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   */
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  /**
   * @brief highlight the currently active ::Verse m_currVerse in the
   * QuranPageBrowser and the side panel
   */
  void highlightCurrentVerse();

public slots:
  /**
   * @brief showVOTDmessage - show the verse of the day dialog
   * @param votd - QPair of the ::Verse of the day and the verse text
   */
  void showVOTDmessage(QPair<Verse, QString> votd);
  /**
   * @brief check if there are updates using the maintainence tool if available,
   * otherwise check latest version on github
   */
  void checkForUpdates();
  /**
   * @brief callback function that handles the output of the maintainence tool
   * update check
   */
  void updateProcessCallback();
  /**
   * @brief save the current position and window state of the application to the
   * settings file
   */
  void saveReaderState();
  /**
   * @brief restart the application
   */
  void restartApp();

protected:
  /**
   * @brief re-implementation of QWidget::resizeEvent(QResizeEvent*) to resize
   * the NotificationPopup size accordingly
   * @param event
   */
  void resizeEvent(QResizeEvent* event);

private slots:
  /**
   * @brief navigates to the next page relative to the current page
   */
  void nextPage();
  /**
   * @brief navigates to the previous page relative to the current page
   */
  void prevPage();
  /**
   * @brief displays the given page and sets the current verse to the
   * 1st verse in the page
   * @param page - page to navigate to
   * @param automaticFlip - boolean indicating whether the function was called
   * by internal signal to automatically flip the page
   */
  void gotoPage(int page, bool automaticFlip = false);
  /**
   * @brief gets the page of the 1st verse in this surah, moves to that page,
   * and starts playback of the surah.
   * @param surahIdx - surah number (1-114)
   */
  void gotoSurah(int surahIdx);
  /**
   * @brief sets the verse combobox values according to the current surah verse
   * count, sets the current verse as the visible index
   * @param forceUpdate - boolean flag forces the combobox to update it's surah
   * verse count
   */
  void setVerseComboBoxRange(bool forceUpdate = false);

  /**
   * @brief  continues playback of the current verse
   */
  void btnPlayClicked();
  /**
   * @brief pause playback of the current verse
   */
  void btnPauseClicked();
  /**
   * @brief stops playback, sets the current verse to the 1st in the page and
   * update verses combobox & selected surah
   */
  void btnStopClicked();

  /**
   * @brief slot for updating the reader page as the user selects a different
   * page from the combobox.
   * @param newPageIdx - the selected idx in the combobox (0 - 603)
   */
  void cmbPageChanged(int newPageIdx);
  /**
   * @brief slot for updating the reader page as the user selects a different
   * verse in the same surah.
   * @param newVerseIdx verse idx in the combobox (0 to (surahVerseCount-1))
   */
  void cmbVerseChanged(int newVerseIdx);
  /**
   * @brief slot for updating the reader page as the user selects a different
   * juz in the same surah.
   * @param newJuzIdx - juz idx in the combobox (0 - 29)
   */
  void cmbJuzChanged(int newJuzIdx);

  /**
   * @brief sync the main window with the verse player as active verse changes,
   * set the endOfPage flag or flip page if the flag is set
   */
  void activeVerseChanged();
  /**
   * @brief display warning message box in case that recitation files are
   * missing
   */
  void missingRecitationFileWarn(int reciterIdx, int surah);
  /**
   * @brief sets the current position in the audio file as the position of the
   * slider.
   * @param position - position in audio file in milliseconds
   */
  void mediaPosChanged(qint64 position);
  /**
   * @brief disables/enables control buttons according to the media player
   * state.
   * @param state - playback state of the current audio file
   */
  void mediaStateChanged(QMediaPlayer::PlaybackState state);

  /**
   * @brief adds the current ::Verse to the bookmarks
   */
  void addCurrentToBookmarks();
  /**
   * @brief toggle play/pause of the current verse
   */
  void spaceKeyPressed();
  /**
   * @brief open the SettingsDialog and connect settings change slots
   */
  void actionPrefTriggered();
  /**
   * @brief open the DownloaderDialog, create instance if not set
   */
  void actionDMTriggered();
  /**
   * @brief open the BookmarksDialog, create instance if not set
   */
  void actionBookmarksTriggered();
  /**
   * @brief open the TafsirDialog for the current ::Verse
   */
  void actionTafsirTriggered();
  /**
   * @brief open the verse of the day dialog
   */
  void actionVotdTriggered();
  /**
   * @brief open the SearchDialog, create instance if not set
   */
  void actionSearchTriggered();
  /**
   * @brief open the about messagebox for the application
   */
  void actionAboutTriggered();
  /**
   * @brief open the about messagebox for Qt
   */
  void on_actionAbout_Qt_triggered();

  /**
   * @brief slot to navigate to the clicked verse in the side panel and update
   * UI elements
   */
  void verseClicked();
  /**
   * @brief navigate to the surah with the given QModelIndex in the surahs list
   * model
   */
  void navigateToSurah(QModelIndex& index);
  /**
   * @brief open TafsirDialog with the shown verse set to the given ::Verse
   * @param v - ::Verse to show the tafsir of
   */
  void showVerseTafsir(Verse v);
  /**
   * @brief navigate to the given ::Verse and update UI elements accordingly
   * @param v - ::Verse to navigate to
   */
  void navigateToVerse(Verse v);
  /**
   * @brief callback function for clicking verses in the QuranPageBrowser that
   * takes actions based on the chosen option in the menu.
   * @param hrefUrl - "#idx" where idx is the verse index relative to the start
   * of the page
   */
  void verseAnchorClicked(const QUrl& hrefUrl);
  /**
   * @brief copy to clipboard the text of the verse with the given index
   * @param IdxInPage - verse index relative to the start of the page
   */
  void copyVerseText(int IdxInPage);

  /**
   * @brief redraw the current Quran page
   * @param manualSz - boolean flag to force the use of the manually set
   * fontsize
   */
  void redrawQuranPage(bool manualSz = false);
  /**
   * @brief updates the side panel with the translation of the current page
   * verses
   */
  void addSideContent();
  /**
   * @brief set the systray icon tooltip text according to the currently playing
   * recitation
   * @param newState - playback state of the current verse
   */
  void updateTrayTooltip(QMediaPlayer::PlaybackState newState);

  /**
   * @brief set tafsir to the one in the settings, update the selected db
   */
  void updateLoadedTafsir();
  /**
   * @brief set translation to the one in the settings, update the selected db
   */
  void updateLoadedTranslation();
  /**
   * @brief set side content font to the one in the settings
   */
  void updateSideFont();

  /**
   * @brief search for the surahs with the given argument when the text in the
   * side dock search box is changed
   * @param arg1 - QString of the new text in the search box
   */
  void searchSurahTextChanged(const QString& arg1);
  /**
   * @brief navigate to the surah clicked in the QListView in the side dock,
   * index might differ than the actual surah number
   * @param index - QModelIndex of the clicked surah
   */
  void listSurahNameClicked(const QModelIndex& index);
  /**
   * @brief change the player volume level as the volume slider changes
   * @param position - position in the slider (0 - 100)
   */
  void volumeSliderValueChanged(int position);

private:
  QSettings* const m_settings = Globals::settings;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  const QDir m_resources = Globals::themeResources;
  const QString& m_updateToolPath = Globals::updateToolPath;
  const bool m_darkMode = Globals::darkMode;
  /**
   * @brief initalizes different parts used by the app
   */
  void init();
  /**
   * @brief load icons for different UI elements
   */
  void loadIcons();
  /**
   * @brief set the current ::Verse from settings
   */
  void loadSettings();
  /**
   * @brief connects signals and slots for different UI components and
   * shortcuts.
   */
  void setupConnections();
  /**
   * @brief initialize the surahs QListView in the side dock and select the
   * current verse's surah
   */
  void setupSurahsDock();
  /**
   * @brief set the QPushButton the menubar that toggles the navigation dock and
   * connect to the appropriate menubar action
   */
  void setupMenubarToggle();
  /**
   * @brief updates the selected surah in the navigation dock to match the
   * active verse in the VersePlayer and navigate to it if different than the
   * currently selected surah
   */
  void updateSurah();
  /**
   * @brief updates the list that contains::Verse instances for verses in the
   * current page
   */
  void updatePageVerseInfoList();
  /**
   * @brief sets m_currVerse to the first verse in m_vInfoList
   */
  void setVerseToStartOfPage();
  /**
   * @brief set the index of the page combobox without signalling other slots
   * @param idx - new index to set the combobox to
   */
  void setCmbPageIdx(int idx);
  /**
   * @brief set the index of the verse combobox without signalling other slots
   * @param idx - new index to set the combobox to
   */
  void setCmbVerseIdx(int idx);
  /**
   * @brief set the index of the juz combobox without signalling other slots
   * @param idx - new index to set the combobox to
   */
  void setCmbJuzIdx(int idx);
  /**
   * @brief boolean indicating if the current verse is the last in the current
   * page or not (not the last verse in the surah)
   */
  bool m_endOfPage = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of page combobox index
   */
  bool m_internalPageChange = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of surah combobox index
   */
  bool m_internalSurahChange = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of verse combobox index
   */
  bool m_internalVerseChange = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of juz combobox index
   */
  bool m_internalJuzChange = false;
  /**
   * @brief float value of the current playback volume (0 - 1.0)
   */
  qreal m_volume = 1;
  /**
   * @brief Pointer to access ui elements generated from .ui files.
   */
  Ui::MainWindow* ui;
  /**
   * @brief pointer to QuranPageBrowser instance
   */
  QuranPageBrowser* m_quranBrowser = nullptr;
  /**
   * @brief pointer to NotificationManager instance
   */
  NotificationManager* m_notifyMgr = nullptr;
  /**
   * @brief pointer to NotificationPopup instance
   */
  NotificationPopup* m_popup = nullptr;
  /**
   * @brief pointer to DBManager instance
   */
  DBManager* m_dbMgr = nullptr;
  /**
   * @brief pointer to VersePlayer instance
   */
  VersePlayer* m_player = nullptr;
  /**
   * @brief pointer to TafsirDialog instance
   */
  TafsirDialog* m_tafsirDlg = nullptr;
  /**
   * @brief pointer to SearchDialog instance
   */
  SearchDialog* m_searchDlg = nullptr;
  /**
   * @brief pointer to SettingsDialog instance
   */
  SettingsDialog* m_settingsDlg = nullptr;
  /**
   * @brief pointer to BookmarksDialog instance
   */
  BookmarksDialog* m_bookmarksDlg = nullptr;
  /**
   * @brief pointer to DownloaderDialog instance
   */
  DownloaderDialog* m_downloaderDlg = nullptr;
  /**
   * @brief pointer to DownloadManager instance
   */
  DownloadManager* m_downManPtr = nullptr;
  /**
   * @brief pointer to the currently highlighted VerseFrame in the side panel
   */
  VerseFrame* m_highlightedFrm = nullptr;
  /**
   * @brief the currently selected ::Verse
   */
  Verse m_currVerse{ 1, 1, 1 };
  /**
   * @brief QList of QFrame pointers to VerseFrame elements in the side panel
   */
  QList<QFrame*> m_verseFrameList;
  /**
   * @brief QList of ::Verse instances for the verses in the current page
   */
  QList<Verse> m_vInfoList;
  /**
   * @brief pointer to the QProcess instance of the maintainence tool that
   * checks for updates
   */
  QProcess* m_process = nullptr;
  /**
   * @brief pointer to the validator for the editable verse combobox to ensure
   * the number entered is within the surah verse range
   */
  QIntValidator* m_verseValidator = nullptr;
  /**
   * @brief QList for surah names as it appears in the navigation dock QListView
   */
  QStringList m_surahList;
  /**
   * @brief model used with the navigation dock QListView to display complete
   * list of surahs
   */
  QStringListModel m_surahListModel;
  /**
   * @brief QFont used in the side panel translation
   */
  QFont m_sideFont;
};
#endif // MAINWINDOW_H
