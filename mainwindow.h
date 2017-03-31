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

#include "robotconfigform.h"
#include "simulationconfigform.h"
#include "evolutionconfigform.h"
#include "configoverviewform.h"
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

public slots:
    void onProjectTreeSelect(QTreeWidgetItem*,int);
    void onProjectTreeAdd();
    void onProjectTreeRemove();

    void loadSimulation();
    void loadEvolution();

    void saveSimulation();
    void saveEvolution();
    void saveRobot();

    void onPushEvolve();
    void onPushSimulate();
    void onPushAnalyze();
    void onPushStop();
    void onFileChanged(QString filename);
    void onNewFileList(QStringList files);

    void evolveReadReady();
    void onEvolveFinished(int);

    void onFileEvent(const QString &);

    void onNewProject();
    void onOpenProject();
    void onSaveProject();

private:
    Ui::MainWindow *ui;


    QString project_path;

    QProcess *process_server;
    QProcess *process_evolve;
    QProcess *process_simulate;

    QList<QProcess*> process_list;

    QFileSystemWatcher *fs_watcher = nullptr;

    int n_generation;

    QString project_directory = "";

    QFileSystemWatcher* tmp_directory_watcher = nullptr;

    SimulationConfigForm *simulation_config_form = nullptr;
    EvolutionConfigForm* evolution_config_form = nullptr;
    RobotConfigForm* robot_config_form = nullptr;
    ConfigOverviewForm* overview_form = nullptr;
    SettingsWindow* settings_window = nullptr;
};

#endif // MAINWINDOW_H
