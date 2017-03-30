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

private:
    Ui::EvolutionConfigForm *ui;

private slots:
    void onComboChanged(QString txt);
};

#endif // EVOLUTIONCONFIGFORM_H
