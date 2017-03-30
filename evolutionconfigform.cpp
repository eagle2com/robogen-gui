#include "evolutionconfigform.h"
#include "ui_evolutionconfigform.h"
#include <QDebug>
#include <QCheckBox>

EvolutionConfigForm::EvolutionConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EvolutionConfigForm)
{
    ui->setupUi(this);

    for(auto& child: findChildren<QComboBox*>()) {
        connect(child, SIGNAL(currentTextChanged(QString)), this, SLOT(onComboChanged(QString)));
    }
}

EvolutionConfigForm::~EvolutionConfigForm()
{
    delete ui;
}

void EvolutionConfigForm::saveAll()
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

void EvolutionConfigForm::loadAll()
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

void EvolutionConfigForm::onComboChanged(QString txt)
{
    if(current_config) {
        current_config->combo_map[sender()->objectName()] = txt;
       // qDebug() << sender() << " :" << txt;
    }
}
