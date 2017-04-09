#ifndef SIMULATIONCONFIGFORM_H
#define SIMULATIONCONFIGFORM_H

#include <QWidget>
#include "configurations.h"

namespace Ui {
class SimulationConfigForm;
}

class SimulationConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationConfigForm(QWidget *parent = 0);
    ~SimulationConfigForm();
    SimulationConfiguration* current_config = nullptr;
    void saveAll();
    void loadAll();
    void writeSimulation(const QString& project_path);

public:
    Ui::SimulationConfigForm *ui;
};

#endif // SIMULATIONCONFIGFORM_H
