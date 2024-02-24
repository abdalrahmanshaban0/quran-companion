#ifndef CONTENTJOB_H
#define CONTENTJOB_H

#include "../types/tafsir.h"
#include "../types/translation.h"
#include "downloadjob.h"
#include "taskdownloader.h"

class ContentJob : public DownloadJob
{
public:
  ContentJob(Type type, int idx);
  ~ContentJob();

  void start() override;
  void stop() override;
  bool isDownloading() override;
  int completed() override;
  int total() override;
  Type type() override;
  QString name() override;

private:
  QList<QSharedPointer<Tafsir>>& m_tafasir = Tafsir::tafasir;
  QList<QSharedPointer<Translation>>& m_translations =
    Translation::translations;
  QPointer<TaskDownloader> m_taskDlr;
  QNetworkAccessManager m_netMgr;
  DownloadTask* m_task;
  Type m_type;
  bool m_isDownloading;
  int m_idx;
};

#endif // CONTENTJOB_H
