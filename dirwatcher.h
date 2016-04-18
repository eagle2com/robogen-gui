#ifndef DIRWATCHER_H
#define DIRWATCHER_H

#include <QDir>
#include <QTimer>
#include <QObject>

class DirWatcher: public QObject
{
    Q_OBJECT
public:
    explicit DirWatcher(QObject *parent = nullptr);
    ~DirWatcher();
    void watch(QString path, int ms = 1000);

signals:
    void onNewFile(QStringList filename);

private slots:
    void onTimer();

private:
    QTimer timer;
    QString path;
};

#endif // DIRWATCHER_H
