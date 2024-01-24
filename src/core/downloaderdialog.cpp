/**
 * @file downloaderdialog.cpp
 * @brief Implementation file for DownloaderDialog
 */

#include "downloaderdialog.h"
#include "ui_downloaderdialog.h"

DownloaderDialog::DownloaderDialog(QWidget* parent, DownloadManager* downloader)
  : QDialog(parent)
  , ui(new Ui::DownloaderDialog)
  , m_downloaderPtr{ downloader }
  , m_surahDisplayNames{ m_dbMgr->surahNameList() }

{
  ui->setupUi(this);
  setWindowIcon(Globals::awesome->icon(fa::fa_solid, fa::fa_download));

  // treeview setup
  QStringList headers;
  headers.append(tr("Name"));
  headers.append(tr("Number"));
  m_treeModel.setHorizontalHeaderLabels(headers);
  ui->treeView->setModel(&m_treeModel);
  populateTreeModel();
  ui->treeView->resizeColumnToContents(0);

  // connectors
  setupConnections();
}

void
DownloaderDialog::setupConnections()
{
  connect(ui->btnAddToQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::addToQueue);

  connect(ui->btnDownloads,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::openDownloadsDir);

  connect(ui->btnStopQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::btnStopClicked);

  connect(ui->btnClearQueue,
          &QPushButton::clicked,
          this,
          &DownloaderDialog::clearQueue);

  connect(m_downloaderPtr,
          &DownloadManager::downloadCompleted,
          this,
          &DownloaderDialog::downloadCompleted);

  connect(m_downloaderPtr,
          &DownloadManager::filesFound,
          this,
          &DownloaderDialog::downloadCompleted);

  connect(m_downloaderPtr,
          &DownloadManager::downloadCanceled,
          this,
          &DownloaderDialog::downloadAborted);

  connect(m_downloaderPtr,
          &DownloadManager::downloadErrored,
          this,
          &DownloaderDialog::topTaskDownloadError);

  connect(m_downloaderPtr,
          &DownloadManager::downloadSpeedUpdated,
          this,
          &DownloaderDialog::updateDownloadSpeed);
}

void
DownloaderDialog::populateTreeModel()
{
  // add reciters
  for (const Reciter& reciter : m_recitersList) {
    QStandardItem* item = new QStandardItem(reciter.displayName);
    item->setToolTip(reciter.displayName);
    m_treeModel.invisibleRootItem()->appendRow(item);

    for (int j = 1; j <= 114; j++) {
      QStandardItem* suraItem =
        new QStandardItem(m_surahDisplayNames.at(j - 1));

      QList<QStandardItem*> rw;
      rw.append(suraItem);
      rw.append(new QStandardItem(QString::number(j)));

      item->appendRow(rw);
    }
  }
  // tafsir submenu
  QStandardItem* tafsir =
    new QStandardItem(qApp->translate("SettingsDialog", "Tafsir"));
  tafsir->setData("tafsir", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(tafsir);
  // -- tafasir
  for (int i = 0; i < m_tafasirList.size(); i++) {
    const Tafsir& t = m_tafasirList.at(i);
    if (!t.extra)
      continue;
    QStandardItem* item = new QStandardItem(t.displayName);
    item->setData("tadb", Qt::UserRole);
    item->setData(i, Qt::UserRole + 1);
    tafsir->appendRow(item);
  }

  // translation submenu
  QStandardItem* translation =
    new QStandardItem(qApp->translate("SettingsDialog", "Translation"));
  tafsir->setData("translation", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(translation);
  // -- translations
  for (int i = 0; i < m_trList.size(); i++) {
    const Translation& tr = m_trList.at(i);
    if (!tr.extra)
      continue;
    QStandardItem* item = new QStandardItem(tr.displayName);
    item->setData("trdb", Qt::UserRole);
    item->setData(i, Qt::UserRole + 1);
    translation->appendRow(item);
  }

  // extras submenu
  QStandardItem* extras = new QStandardItem(tr("Extras"));
  extras->setToolTip(tr("Additional files"));
  extras->setData("extras", Qt::UserRole);
  m_treeModel.invisibleRootItem()->appendRow(extras);
  // -- qcf 2
  QStandardItem* qcf =
    new QStandardItem(qApp->translate("SettingsDialog", "QCF V2"));
  qcf->setData("qcf", Qt::UserRole);
  extras->appendRow(qcf);
}

void
DownloaderDialog::addToDownloading(int reciter, int surah)
{
  // add surah to downloading tasks
  QSet<int>& downloading = m_downloadingTasks[reciter];
  downloading.insert(surah);
}

void
DownloaderDialog::removeFromDownloading(int reciter, int surah)
{
  QSet<int>& downloading = m_downloadingTasks[reciter];
  downloading.remove(surah);
  if (downloading.isEmpty())
    m_downloadingTasks.remove(reciter);
}

void
DownloaderDialog::addToQueue()
{
  static int recitersnum = m_recitersList.size();
  QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

  foreach (const QModelIndex& i, selected) {
    int parent = i.parent().row();
    int current = i.row();
    bool toplevel = parent < 0;

    // recitation (reciter selected)
    if (toplevel && current < recitersnum) {
      for (int surah = 1; surah <= 114; surah++)
        enqueueSurah(current, surah);
    }
    // recitation (surah index selected)
    else if (!toplevel && parent < recitersnum)
      enqueueSurah(parent, current + 1);
    // tafasir
    else if (i.data(Qt::UserRole).toString() == "tadb") {
      QPair<int, int> info(0, i.data(Qt::UserRole + 1).toInt());
      m_downloaderPtr->addToQueue(File, info);
      addTaskProgress(File, info);
    }
    // translation
    else if (i.data(Qt::UserRole).toString() == "trdb") {
      QPair<int, int> info(1, i.data(Qt::UserRole + 1).toInt());
      m_downloaderPtr->addToQueue(File, info);
      addTaskProgress(File, info);
    }
    // extras
    else if (i.data(Qt::UserRole).toString() == "qcf") {
      m_downloaderPtr->addToQueue(QCF);
      addTaskProgress(QCF);
    }
  }

  setCurrentBar();
  m_downloaderPtr->startQueue();
}

void
DownloaderDialog::addTaskProgress(DownloadType type, QPair<int, int> info)
{
  int total = 0;
  QString objName;
  if (type == Recitation) {
    QString reciter = m_recitersList.at(info.first).displayName;
    QString surahName = m_surahDisplayNames.at(info.second - 1);
    objName = reciter + tr(" // Surah: ") + surahName;
    total = m_dbMgr->getSurahVerseCount(info.second);
  } else if (type == QCF) {
    objName = qApp->translate("SettingsDialog", "QCF V2");
    total = 604;
  } else if (type == File) {
    objName = info.first ? m_trList.at(info.second).displayName
                         : m_tafasirList.at(info.second).displayName;
  }

  QFrame* prgFrm = new QFrame(ui->scrollAreaWidgetContents);
  prgFrm->setLayout(new QVBoxLayout);
  prgFrm->setObjectName(objName);

  QBoxLayout* downInfo;
  if (m_languageCode == QLocale::Arabic)
    downInfo = new QBoxLayout(QBoxLayout::RightToLeft, prgFrm);
  else
    downInfo = new QHBoxLayout(prgFrm);

  QLabel* lbTitle = new QLabel(prgFrm);
  lbTitle->setObjectName("DownloadInfo");
  lbTitle->setText(prgFrm->objectName());
  QLabel* downSpeed = new QLabel(prgFrm);
  downSpeed->setObjectName("DownloadSpeed");
  downSpeed->setAlignment(Qt::AlignRight);

  downInfo->addWidget(lbTitle);
  downInfo->addWidget(downSpeed);
  prgFrm->layout()->addItem(downInfo);

  DownloadProgressBar* dpb = new DownloadProgressBar(prgFrm, type, total);
  prgFrm->layout()->addWidget(dpb);
  m_frameLst.append(prgFrm);

  ui->lytFrameView->addWidget(prgFrm);
}

void
DownloaderDialog::enqueueSurah(int reciter, int surah)
{
  bool currentlyDownloading = m_downloadingTasks.value(reciter).contains(surah);
  if (currentlyDownloading)
    return;

  addToDownloading(reciter, surah);
  addTaskProgress(Recitation, QPair<int, int>(reciter, surah));
  m_downloaderPtr->addToQueue(reciter, surah);
}

void
DownloaderDialog::setCurrentBar()
{
  if (m_frameLst.empty())
    return;

  m_currentLb = m_frameLst.at(0)->findChild<QLabel*>("DownloadInfo");
  m_currDownSpeedLb = m_frameLst.at(0)->findChild<QLabel*>("DownloadSpeed");
  m_currentLb->setText(tr("Downloading: ") +
                       m_currentLb->parent()->objectName());
  m_currentBar = m_frameLst.at(0)->findChild<DownloadProgressBar*>();

  connect(m_downloaderPtr,
          &DownloadManager::downloadProgressed,
          m_currentBar,
          &DownloadProgressBar::updateProgress);
}

void
DownloaderDialog::updateDownloadSpeed(int value, QString unit)
{
  m_currDownSpeedLb->setText(QString::number(value) + " " + unit + tr("/sec"));
}

void
DownloaderDialog::selectDownload(DownloadType type, QPair<int, int> info)
{
  QItemSelectionModel* selector = ui->treeView->selectionModel();
  QModelIndex parent;
  QModelIndex task;
  if (type == Recitation) {
    parent = m_treeModel.index(info.first, 0);
    task = m_treeModel.index(info.second - 1, 0, parent);
  } else if (type == QCF) {
    parent = m_treeModel.index(m_treeModel.rowCount() - 1, 0);
    task = m_treeModel.index(0, 0, parent);
  } else if (type == File) {
    parent = m_treeModel.index(m_treeModel.rowCount() - 2 - !info.first, 0);
    // remove default db indices from current index as defaults are not
    // downloadable
    if (!info.first)
      // tafsir
      info.second -= info.second > 6;
    else
      // translation
      info.second -= 1 + info.second > 5;
    task = m_treeModel.index(info.second, 0, parent);
  }

  ui->treeView->collapseAll();
  ui->treeView->expand(parent);
  selector->clearSelection();
  selector->select(task,
                   QItemSelectionModel::Rows | QItemSelectionModel::Select);
}

void
DownloaderDialog::clearQueue()
{
  m_downloadingTasks.clear();
  m_downloaderPtr->stopQueue();
  if (!m_finishedFrames.isEmpty()) {
    qDeleteAll(m_finishedFrames);
    m_finishedFrames.clear();
  }
}

void
DownloaderDialog::btnStopClicked()
{
  m_downloadingTasks.clear();
  m_downloaderPtr->stopQueue();
}

void
DownloaderDialog::downloadAborted()
{
  m_downloadingTasks.clear();
  if (!m_frameLst.isEmpty()) {
    qDeleteAll(m_frameLst);
    m_frameLst.clear();
  }
}

void
DownloaderDialog::downloadCompleted(DownloadType type,
                                    const QList<int>& metainfo)
{
  m_currentBar->setStyling(DownloadProgressBar::completed);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Completed"));
  disconnect(m_downloaderPtr,
             &DownloadManager::downloadProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  if (type == Recitation)
    removeFromDownloading(metainfo[0], metainfo[1]);
  if (m_currentBar->maximum() == 1)
    m_currentBar->setFormat("1 / 1");
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
}

void
DownloaderDialog::topTaskDownloadError(DownloadType type,
                                       const QList<int>& metainfo)
{
  m_currentBar->setStyling(DownloadProgressBar::aborted);
  m_currentLb->setText(m_currentLb->parent()->objectName());
  m_currDownSpeedLb->setText(tr("Download Failed"));
  disconnect(m_downloaderPtr,
             &DownloadManager::downloadProgressed,
             m_currentBar,
             &DownloadProgressBar::updateProgress);

  if (type == Recitation)
    removeFromDownloading(metainfo[0], metainfo[1]);
  m_finishedFrames.append(m_frameLst.front());
  m_frameLst.pop_front();
  setCurrentBar();
}

void
DownloaderDialog::openDownloadsDir()
{
  QUrl url = QUrl::fromLocalFile(Globals::downloadsDir.absolutePath());
  QDesktopServices::openUrl(url);
}

void
DownloaderDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

DownloaderDialog::~DownloaderDialog()
{
  delete ui;
}
