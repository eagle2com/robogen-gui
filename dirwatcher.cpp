#include "dirwatcher.h"

DirWatcher::DirWatcher(QObject *parent)
{

    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

DirWatcher::~DirWatcher()
{

}

void DirWatcher::watch(QString path, int ms)
{
    this->path = path;
    timer.setInterval(ms);
    timer.start();
}

void DirWatcher::onTimer()
{
    QDir dir(path);
    ///TODO: Fix this quick dirty test
    QStringList list = dir.entryList({"*.json"});
    emit onNewFile(list);
}
