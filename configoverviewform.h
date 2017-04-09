#ifndef CONFIGOVERVIEWFORM_H
#define CONFIGOVERVIEWFORM_H

#include <QWidget>
#include "configurations.h"
#include "robotconfigform.h"
#include "evolutionconfigform.h"
#include "simulationconfigform.h"
#include <QList>
#include <QProcess>

namespace Ui {
class ConfigOverviewForm;
}

class ConfigOverviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigOverviewForm(QWidget *parent = 0);
    ~ConfigOverviewForm();
    ProjectConfiguration* current_config = nullptr;
    void loadAll();
    void saveAll();

    void* main_window = nullptr;

    RobotConfigForm* robot_form = nullptr;
    SimulationConfigForm* simulation_form = nullptr;
    EvolutionConfigForm* evolution_form = nullptr;

   // QString project_path = "";

    void refresh_run_list();
    void refresh_generation_list(const QString &run_name);

private slots:
    void onNameChange();
    void onRunItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

private:
    Ui::ConfigOverviewForm *ui;

};

#endif // CONFIGOVERVIEWFORM_H
