#include "simulationconfigform.h"
#include "ui_simulationconfigform.h"

#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

SimulationConfigForm::SimulationConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulationConfigForm)
{
    ui->setupUi(this);
}

SimulationConfigForm::~SimulationConfigForm()
{
    delete ui;
}

void SimulationConfigForm::saveAll()
{
    if(current_config) {
        for(QComboBox *child: findChildren<QComboBox*>()) {
            current_config->combo_map[child->objectName()] = child->currentText();
        }

        for(QSpinBox* child: findChildren<QSpinBox*>()) {
            current_config->spin_map[child->objectName()] = child->value();
        }

        for(QDoubleSpinBox* child: findChildren<QDoubleSpinBox*>()) {
            current_config->double_spin_map[child->objectName()] = child->value();
        }

        for(QCheckBox* child: findChildren<QCheckBox*>()) {
            current_config->bool_map[child->objectName()] = child->isChecked();
        }
    }
}

void SimulationConfigForm::loadAll()
{
    if(current_config) {
        for(auto& pair: current_config->combo_map) {
            QComboBox* child = findChild<QComboBox*>(pair.first);
            child->setCurrentText(pair.second);
        }

        for(auto& pair: current_config->spin_map) {
            QSpinBox* child = findChild<QSpinBox*>(pair.first);
            child->setValue(pair.second);
        }

        for(auto& pair: current_config->double_spin_map) {
            QDoubleSpinBox* child = findChild<QDoubleSpinBox*>(pair.first);
            child->setValue(pair.second);
        }

        for(auto& pair: current_config->bool_map) {
            QCheckBox* child = findChild<QCheckBox*>(pair.first);
            child->setChecked(pair.second);
        }
    }
}
