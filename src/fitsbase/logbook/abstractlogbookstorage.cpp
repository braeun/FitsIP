#include "abstractlogbookstorage.h"
#include "logbookentry.h"
#include "logbookfilter.h"

AbstractLogbookStorage::AbstractLogbookStorage():
  idcounter(0),
  title(""),
  desc("")
{
}

AbstractLogbookStorage::~AbstractLogbookStorage()
{
}

bool AbstractLogbookStorage::add(LogbookEntry& entry)
{
  setId(entry,++idcounter);
  entries.push_back(entry);
  if (!entry.getImage().isEmpty()) images.insert(entry.getImage());
  projects.insert(entry.getProject());
  steps[entry.getProject()].insert(entry.getStep());
  write();
  return true;
}

bool AbstractLogbookStorage::update(const LogbookEntry& entry)
{
  for (auto& e : entries)
  {
    if (e.getId() == entry.getId())
    {
      e = entry;
      break;
    }
  }
  write();
  return true;
}

bool AbstractLogbookStorage::remove(int64_t id)
{
  for (size_t i=0;i<entries.size();++i)
  {
    if (entries[i].getId() == id)
    {
      entries.erase(entries.begin()+i);
      break;
    }
  }
  write();
  return true;
}



void AbstractLogbookStorage::setTitle(const QString &t)
{
  title = t;
  write();
}

QString AbstractLogbookStorage::getTitle() const
{
  return title;
}

void AbstractLogbookStorage::setDescription(const QString &d)
{
  desc = d;
  write();
}

QString AbstractLogbookStorage::getDescription() const
{
  return desc;
}

std::vector<LogbookEntry> AbstractLogbookStorage::getEntries() const
{
  return entries;
}

std::vector<LogbookEntry> AbstractLogbookStorage::getEntries(const LogbookFilter& filter) const
{
  std::vector<LogbookEntry> list;
  for (const auto& e : entries)
  {
    if (filter.check(e)) list.push_back(e);
  }
  return list;
}

const LogbookEntry& AbstractLogbookStorage::getLastEntry() const
{
  if (!entries.empty()) return entries.back();
  return LogbookEntry::invalid;
}

LogbookEntry AbstractLogbookStorage::getEntry(int64_t id) const
{
  for (const auto& e : entries)
  {
    if (e.getId() == id) return e;
  }
  return LogbookEntry::invalid;
}

std::set<QString> AbstractLogbookStorage::getImages() const
{
  return images;
}

std::set<QString> AbstractLogbookStorage::getProjects() const
{
  return projects;
}

std::set<QString> AbstractLogbookStorage::getSteps(const QString& project) const
{
  if (project.isEmpty())
  {
    std::set<QString> set;
    for (const auto& entry : steps)
    {
      set.insert(entry.second.begin(),entry.second.end());
      return set;
    }
  }
  if (steps.find(project) == steps.end()) return std::set<QString>{"generic"};
  return steps.at(project);
}

void AbstractLogbookStorage::getTimeRange(QDateTime* begin, QDateTime* end) const
{
  if (entries.empty())
  {
    if (begin) *begin = QDateTime::currentDateTime();
    if (end) *end = QDateTime::currentDateTime();
  }
  if (begin)
  {
    QDateTime t = entries.front().getTimestamp();
    for (const auto& entry : entries)
    {
      if (entry.getTimestamp() < t) t = entry.getTimestamp();
    }
    *begin = t;
  }
  if (end)
  {
    *end = getLastEntry().getTimestamp();
  }
}



void AbstractLogbookStorage::clear()
{
  idcounter = 0;
  entries.clear();
  images.clear();
  projects.clear();
  projects.insert("generic");
  steps.clear();
}

