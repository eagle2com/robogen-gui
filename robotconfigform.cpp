#include "robotconfigform.h"
#include "ui_robotconfigform.h"
#include <QDebug>

RobotConfigForm::RobotConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RobotConfigForm)
{
    ui->setupUi(this);

    RobotPart *part = new RobotPart;
    part->name = "ROOT";
    part->type = PART_TYPE::CORE_COMPONENT;
    part->face = PART_FACE::FRONT;
    part->update();

    root_part = part;

    ui->robotConfigTree->addTopLevelItem(part);

    ui->robotConfigTree->expandAll();

    ui->robotPartCombo->addItem("Core Component");
    ui->robotPartCombo->setItemIcon(0, QIcon(":/imgs/CoreComponent"));

    ui->robotPartCombo->addItem("Fixed Brick");
    ui->robotPartCombo->setItemIcon(1, QIcon(":/imgs/FixedBrick"));

    ui->robotPartCombo->addItem("Active Hinge");
    ui->robotPartCombo->setItemIcon(2, QIcon(":/imgs/ActiveHinge"));

    ui->robotPartCombo->addItem("Passive Hinge");
    ui->robotPartCombo->setItemIcon(3, QIcon(":/imgs/PassiveHinge"));

    ui->robotPartCombo->addItem("Parametric Joint");
    ui->robotPartCombo->setItemIcon(4, QIcon(":/imgs/ParametricJoint"));

    ui->robotPartCombo->addItem("Light Sensor");
    ui->robotPartCombo->setItemIcon(5, QIcon(":/imgs/LightSensor"));

    ui->robotPartCombo->addItem("Ir Sensor");
    ui->robotPartCombo->setItemIcon(6, QIcon(":/imgs/IrSensor"));

    ui->robotPartCombo->addItem("Active Wheel");
    ui->robotPartCombo->setItemIcon(7, QIcon(":/imgs/ActiveHinge"));

    ui->robotPartCombo->addItem("Passive Wheel");
    ui->robotPartCombo->setItemIcon(8, QIcon(":/imgs/PassiveHinge"));

    //FIXME move this
    //ui->spin_threads->setMinimum(1);
    //ui->spin_threads->setMaximum(QThread::idealThreadCount()-1);
    //ui->spin_threads->setValue(1);

    connect(ui->robotPartCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onRobotPartChange(int)));
    connect(ui->robotFaceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onRobotFaceChange(int)));
    connect(ui->robotRotateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onRobotRotationChange(int)));
    connect(ui->spin_paramlength, SIGNAL(valueChanged(double)), this, SLOT(onRobotParamLengthChange(double)));
    connect(ui->spin_paramrotation, SIGNAL(valueChanged(int)), this, SLOT(onRobotParamRotationChange(int)));
    connect(ui->button_add_part, SIGNAL(clicked(bool)), this, SLOT(onAddPart()));
    connect(ui->button_remove_part, SIGNAL(clicked(bool)), this, SLOT(onRemovePart()));
    connect(ui->robotConfigTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui->robotPartNameEdit, SIGNAL(textEdited(QString)), this, SLOT(onRobotPartNameChange(QString)));
}

void RobotConfigForm::onItemChange(QTreeWidgetItem*, QTreeWidgetItem*)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    ui->robotFaceCombo->blockSignals(true);
    ui->robotPartCombo->blockSignals(true);
    ui->robotRotateCombo->blockSignals(true);
    ui->robotPartNameEdit->blockSignals(true);
    ui->spin_paramlength->blockSignals(true);
    ui->spin_paramrotation->blockSignals(true);

    ui->robotPartCombo->setCurrentIndex((int)part->type);
    ui->robotFaceCombo->setCurrentIndex((int)part->face);
    ui->robotRotateCombo->setCurrentIndex((int)part->rotation);
    ui->robotPartNameEdit->setText(part->name);
    ui->spin_paramlength->setValue(part->param_length);
    ui->spin_paramrotation->setValue(part->param_rotation);

    ui->robotFaceCombo->blockSignals(false);
    ui->robotPartCombo->blockSignals(false);
    ui->robotRotateCombo->blockSignals(false);
    ui->robotPartNameEdit->blockSignals(false);
    ui->spin_paramlength->blockSignals(false);
    ui->spin_paramrotation->blockSignals(false);
}

void RobotConfigForm::onAddPart()
{
    RobotPart *part = new RobotPart;
    root_part->addChild(part);
    ui->robotConfigTree->clearSelection();
    ui->robotConfigTree->setCurrentItem(part);
}

void RobotConfigForm::onRemovePart()
{
    QTreeWidgetItem* item = ui->robotConfigTree->currentItem();
    printf("item: %p\n", item);
    if(!item)
        return;

    if(item == root_part)
        return;

    if(!item->parent())
        return;

    QModelIndex index =  ui->robotConfigTree->currentIndex();
    QModelIndex index2 = index.sibling(index.row() - 1, 0);
    ui->robotConfigTree->setCurrentIndex(index2);
    item->parent()->removeChild(item);
}


void RobotConfigForm::onRobotPartChange(int index)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->type = (PART_TYPE)index;
    part->update();
}

void RobotConfigForm::onRobotFaceChange(int index)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->face = (PART_FACE)index;
    part->update();
}

void RobotConfigForm::onRobotPartNameChange(QString name)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->name = name;
    part->update();
}

void RobotConfigForm::onRobotRotationChange(int index)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->rotation = index;
    part->update();
}

void RobotConfigForm::onRobotParamLengthChange(double value)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_length = value;
    }
}

void RobotConfigForm::onRobotParamRotationChange(int value)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_rotation = value;
    }
}


PART_TYPE RobotConfigForm::typeFromString(QString str)
{

    PART_TYPE type = PART_TYPE::PASSIVE_HINGE;

    if(str == "FixedBrick")
        type = PART_TYPE::FIXED_BRICK;

    else if(str == "ActiveHinge")
        type = PART_TYPE::ACTIVE_HINGE;

    else if(str == "PassiveHinge")
        type = PART_TYPE::PASSIVE_HINGE;

    else if(str == "IrSensor")
        type = PART_TYPE::IR_SENSOR;

    else if(str == "LightSensor")
        type = PART_TYPE::LIGHT_SENSOR;

    else if(str == "ParametricJoint")
        type = PART_TYPE::PARAMETRIC_JOINT;

    else if(str == "CoreComponent")
        type = PART_TYPE::CORE_COMPONENT;

    else if(str == "ActiveWheel")
        type = PART_TYPE::ACTIVE_WHEEL;

    else if(str == "PassiveWheel")
        type = PART_TYPE::PASSIVE_WHEEL;
    else {
        qDebug() << "ERROR: Part type '" << str << "' is  unupported";
    }

    return type;

}

RobotConfigForm::~RobotConfigForm()
{
    delete ui;
}
