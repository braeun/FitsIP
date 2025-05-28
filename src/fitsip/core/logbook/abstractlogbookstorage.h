#ifndef ABSTRACTLOGBOOKSTORAGE_H
#define ABSTRACTLOGBOOKSTORAGE_H

#include "logbookstorage.h"

class AbstractLogbookStorage: public LogbookStorage
{
public:
  AbstractLogbookStorage();
  virtual ~AbstractLogbookStorage();

  virtual bool add(LogbookEntry& entry) override;

  virtual bool update(const LogbookEntry& entry) override;

  virtual bool remove(int64_t id) override;

  virtual void setTitle(const QString& title) override;

  virtual QString getTitle() const override;

  virtual void setDescription(const QString& desc) override;

  virtual QString getDescription() const override;

  virtual std::vector<LogbookEntry> getEntries() const override;

  virtual std::vector<LogbookEntry> getEntries(const LogbookFilter& filter) const override;

  virtual LogbookEntry getLastEntry() const override;

  virtual LogbookEntry getEntry(int64_t id) const override;

  virtual std::set<QString> getImages() const override;

  virtual std::set<QString> getProjects() const override;

  virtual std::set<QString> getSteps(const QString& project) const override;

  virtual void getTimeRange(QDateTime* begin, QDateTime* end) const override;

protected:
  void clear();

  virtual void read() = 0;
  virtual void write() = 0;

private:
  int64_t idcounter;
  std::vector<LogbookEntry> entries;
  std::set<QString> images;
  std::set<QString> projects;
  std::map<QString,std::set<QString>> steps;
  QString title;
  QString desc;

};

#endif // ABSTRACTLOGBOOKSTORAGE_H
