#ifndef EVOLUTIONCONFIGFORM_H
#define EVOLUTIONCONFIGFORM_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include "configurations.h"

namespace Ui {
class EvolutionConfigForm;
}

class EvolutionConfigForm : public QWidget
{
    Q_OBJECT    

public:
    explicit EvolutionConfigForm(QWidget *parent = 0);
    ~EvolutionConfigForm();
    EvolutionConfiguration* current_config = nullptr;
    void saveAll();
    void loadAll();
    void writeEvolution(const QString& project_path, int n_threads);
    int getMaxGenerations();

public:
    Ui::EvolutionConfigForm *ui;

private slots:
    void onComboChanged(QString txt);
};

#endif // EVOLUTIONCONFIGFORM_H
