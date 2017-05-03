#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QProcess>
#include <QHash>
#include <QList>
#include <QFileSystemWatcher>
#include <QJsonObject>
#include "dirwatcher.h"

#include "settingswindow.h"

#include "configurations.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString project_directory = "";
    QString getRobogenPath();

    static QString stringFromType(PART_TYPE tp);
    static PART_TYPE typeFromString(const QString& str);
    QJsonObject getRobotJsonObject();
    //QJsonObject getPartJsonObject(RobotPart* part);

public slots:
    void onProjectTreeSelect(QListWidgetItem*);
    void onProjectTreeAdd();
    void onProjectTreeRemove();

    //void loadSimulation();
    //void loadEvolution();

    void onPushSimulate();
    void onPushWebGL();
    void onPushAnalyze();
    void onPushStop();
    void onFileChanged(QString filename);
    void onNewFileList(QStringList files);

    void evolveReadReady();
    void onEvolveFinished(int);

    void onFileEvent(const QString &);

    void onNewProject();
    void onOpenProject();
    bool onSaveProject();

    void onEvolve();

    void saveSimulation();
    void loadSimulation();

    void saveEvolution();
    void loadEvolution();

    void saveRobot();
    void loadRobot();

private slots:

    void onAddPart();
    void onRemovePart();
    void onRobotPartChange(int index);
    void onRobotFaceChange(int index);
    void onRobotPartNameChange(QString name);
    void onRobotRotationChange(int index);
    void onRobotParamLengthChange(double value);
    void onRobotParamRotationChange(int value);
    void onItemChange(QTreeWidgetItem *, QTreeWidgetItem *);

    void onRunSelect(QListWidgetItem* current);
    void onRunDeletePressed();
    void onRunItemDoubleClicked(QListWidgetItem *item);

    void onProjectConfigDoubleClicked(QListWidgetItem* item);
    void onLoadRobot();
    void loadRobotJson(const QString& filename);

private:
    Ui::MainWindow *ui;

    QFileSystemWatcher *fs_watcher = nullptr;

    int n_generation;

    bool first_dir_change = true;
    bool run_dir_added = false;

    QProcess *process_server;
    QProcess *process_evolve;
    QProcess *process_simulate;

    QList<QProcess*> process_list;
    DirWatcher dir_watcher;

    SettingsWindow* settings_window = nullptr;

    ProjectConfiguration* current_config = nullptr;
    ProjectConfiguration* current_running_config = nullptr;

    QString current_run_name = "";
    QString current_run_path = "";

    void writeEvolution();
    void writeSimulation();
    void writeRobot();

    void closeEvent(QCloseEvent* ev);
};

#endif // MAINWINDOW_H
