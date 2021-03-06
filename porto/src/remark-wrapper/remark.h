#pragma once
#include <QObject>
#include <collection.h>

class QProcess;
class Remark : public QObject
{
  Q_OBJECT
  
public:  
  Remark(soft::Collection *, QString remarcPath, QObject *parent = nullptr);
  virtual ~Remark();

public slots:
  void run();
  void exited();
  void started();
  void readStdout();
  void readStderr();

signals:
  void finished();
  
private:
  QString remarcPath;
  QProcess *process;
  soft::Collection *collection;
};
