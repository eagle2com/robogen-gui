#include "robotconfigform.h"
#include "ui_robotconfigform.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RobotConfigForm::RobotConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RobotConfigForm)
{
    ui->setupUi(this);

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
    ui->robotPartCombo->setItemIcon(7, QIcon(":/imgs/ActiveWheel"));

    ui->robotPartCombo->addItem("Passive Wheel");
    ui->robotPartCombo->setItemIcon(8, QIcon(":/imgs/PassiveWheel"));

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

    connect(ui->push_load, SIGNAL(clicked()), this, SLOT(onLoadRobot()));
}

void RobotConfigForm::onItemChange(QTreeWidgetItem*, QTreeWidgetItem*)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
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

void RobotConfigForm::onLoadRobot()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose robot config file", "", "*.json *.txt");
    //qDebug() << filename;

    QString extension;
    {
        QStringList tokens = filename.split('.');
        extension = tokens.back();
    }

    if(extension == "json")
    {
        loadRobotJson(filename);
        return;
    }

    QFile file(filename);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line;

        //ui->robotConfigTree->clear();
        current_config->robot_part_hash.clear();

        if(current_config->root_part) delete current_config->root_part;
        current_config->root_part = new RobotPart();
        current_config->root_part->type = PART_TYPE::CORE_COMPONENT;

        in.readLine();
        RobotPart* current_parent = current_config->root_part;
        RobotPart *new_part = nullptr;
        RobotPart *prev_part = nullptr;
        int tabs = 1;

        while(in.readLineInto(&line) && !line.isEmpty())
        {
            QStringList tokens = line.split(QRegularExpression("[ \t]"));
            int ctabs = 0;
            //qDebug() << tokens;
            while(tokens.front().isEmpty())
            {
                ctabs++;
                tokens.pop_front();
            }

            new_part = new RobotPart;

            new_part->face = (PART_FACE)tokens.front().toInt(); tokens.pop_front();
            QString type = tokens.front(); tokens.pop_front();

            new_part->type = typeFromString(type);
            new_part->name = tokens.front(); tokens.pop_front();
            new_part->rotation = tokens.front().toInt(); tokens.pop_front();

            if(new_part->type == PART_TYPE::PARAMETRIC_JOINT)
            {
                new_part->param_length = tokens.front().toDouble(); tokens.pop_front();
                new_part->param_rotation = tokens.front().toDouble(); tokens.pop_front();
                // We leave the last 0 to rot because we dont need it
            }

            if(ctabs == tabs)
            {
                current_parent->addChild(new_part);
            }
            else if(ctabs > tabs)
            {
                tabs = ctabs;
                prev_part->addChild(new_part);
                current_parent = prev_part;
            }
            else
            {
                while(ctabs < tabs)
                {
                    current_parent = (RobotPart*)current_parent->parent();
                    tabs--;
                }

                tabs = ctabs;
                current_parent->addChild(new_part);
            }

            new_part->update();
            current_config->robot_part_hash.insert(new_part->name, new_part);
            current_config->root_part->update();

            prev_part = new_part;
        }

        ui->robotConfigTree->addTopLevelItem(current_config->root_part);
        ui->robotConfigTree->expandAll();
    }
    else
    {
        qDebug() << "FAILED TO OPEN ROBOT FILE: " << filename <<endl;
    }
}

void RobotConfigForm::loadRobotJson(const QString &filename)
{
    //qDebug() << "Opening robot json config: " << filename << " ... ";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        //qDebug() << "[FAIL]" << endl;
        QMessageBox::critical(this, "Error", "Could not open " + filename + " for reading.");
        return;
    }
    //qDebug() << "[ OK ]";

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = doc.object();

    QJsonArray parts = object["body"].toObject()["part"].toArray();

    //qDebug() << "Found " << parts.size() << " parts";

    QHash<QString, RobotPart*> hash;

    RobotPart *new_root_part = new RobotPart;
    foreach (const QJsonValue & part, parts)
    {
        RobotPart *new_part = new RobotPart;
        new_part->name = part.toObject()["id"].toString();
        new_part->type = typeFromString(part.toObject()["type"].toString());
        //qDebug() << "type string: " << part.toObject()["type"].toString();
        new_part->rotation = part.toObject()["orientation"].toInt();
        if(new_part->type == PART_TYPE::CORE_COMPONENT)
            new_root_part = new_part;

        //qDebug() << "part: NAME='" << new_part->name << "' type='" << (int)new_part->type << "' rotation='" << new_part->rotation;
        hash.insert(new_part->name, new_part);
    }

    QJsonArray connections = object["body"].toObject()["connection"].toArray();
    foreach (const QJsonValue & connection, connections)
    {
        RobotPart *parent = hash[connection.toObject()["src"].toString()];
        RobotPart *child = hash[connection.toObject()["dest"].toString()];
        child->face = (PART_FACE)connection.toObject()["srcSlot"].toInt();
        parent->addChild(child);
        child->update();
    }
    //qDebug() << "Root part:" << new_root_part->name << endl;
    current_config->root_part = new_root_part;
    current_config->root_part->update();
    ui->robotConfigTree->clear();
    ui->robotConfigTree->addTopLevelItem(current_config->root_part);
    ui->robotConfigTree->expandAll();
}

void RobotConfigForm::loadAll()
{
    //ui->robotConfigTree->clear();
    ui->robotConfigTree->takeTopLevelItem(0);
    ui->robotConfigTree->addTopLevelItem(current_config->root_part);
    ui->robotConfigTree->expandAll();
}

void RobotConfigForm::saveAll()
{

}

QJsonObject RobotConfigForm::getPartJsonObject(RobotPart* part)
{
    /*QJsonObject obj;
    QString type;
    QString name;
    switch(part->type)
    {
    case PART_TYPE::CORE_COMPONENT:
        type = "CoreComponent";
        break;
    case PART_TYPE::FIXED_BRICK:
        type = "FixedBrick";
        break;
    case PART_TYPE::ACTIVE_HINGE:
        type = "ActiveHinge";
        break;
    case PART_TYPE::PASSIVE_HINGE:
        type = "PassiveHinge";
        break;
    case PART_TYPE::PARAMETRIC_JOINT:
        type = "ParametricJoint";
        break;
    case PART_TYPE::LIGHT_SENSOR:
        type = "LightSensor";
        break;
    case PART_TYPE::IR_SENSOR:
        type = "IrSensor";
        break;
    case PART_TYPE::ACTIVE_WHEEL:
        type = "ActiveWheel";
        break;
    case PART_TYPE::PASSIVE_WHEEL:
        type = "PassiveWheel";
        break;
    }


    name = part->name;
    if(name.isEmpty())
        name = "PARTGEN_" + QString::number((ulong)part);


    obj["id"] = name;
    obj["type"] =

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        // The last param is the inclination, which is disabled for now and force to 0
        stream << " " << part->param_length << " " << part->param_rotation <<" 0";
    }
    //FIXME Dirty hack
    if(part->type == PART_TYPE::ACTIVE_WHEEL || part->type == PART_TYPE::PASSIVE_WHEEL)
    {
        // radius?
        stream << " " << 0.04;
    }
    stream << endl;

    for(int i = 0; i < part->childCount(); i++)
    {
        writeRobotPart(dynamic_cast<RobotPart*>(part->child(i)), tab+1,stream);
    }

    return obj;*/
}

QJsonObject RobotConfigForm::getRobotJsonObject()
{
    QJsonObject obj;

    QJsonObject body_obj;

    // Array of parts
    QJsonArray part_array;

    // Array of connections
    QJsonArray conn_array;

    QTreeWidgetItemIterator it(ui->robotConfigTree);
    while(*it) {
        RobotPart* part = dynamic_cast<RobotPart*>(*it);
        QJsonObject part_obj;

        part_obj["id"] = part->name;
        part_obj["type"] = stringFromType(part->type);
        part_obj["root"] = part->parent() == nullptr? true:false;
        part_obj["orientation"] = part->rotation;

        if(part->parent() != nullptr) {
            QJsonObject conn_obj;
            conn_obj["src"] = dynamic_cast<RobotPart*>(part->parent())->name;
            conn_obj["dest"] = part->name;
            conn_obj["srcSlot"] = (int)part->face;
            conn_obj["destSlot"] = 0;
            conn_array.push_back(conn_obj);
        }

        part_array.push_back(part_obj);
        ++it;
    }
    body_obj["part"] = part_array;
    body_obj["connection"] = conn_array;

    obj["body"] = body_obj;

    return obj;
}

void RobotConfigForm::onAddPart()
{
    if(!current_config)
        return;
    RobotPart *part = new RobotPart;
    current_config->root_part->addChild(part);
    ui->robotConfigTree->clearSelection();
    ui->robotConfigTree->setCurrentItem(part);
}

void RobotConfigForm::onRemovePart()
{
    QTreeWidgetItem* item = ui->robotConfigTree->currentItem();
    //printf("item: %p\n", item);
    if(!item)
        return;

    if(item == current_config->root_part)
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
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    part->face = (PART_FACE)index;
    part->update();
}

void RobotConfigForm::onRobotPartNameChange(QString name)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    part->name = name;
    part->update();
}

void RobotConfigForm::onRobotRotationChange(int index)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    part->rotation = index;
    part->update();
}

void RobotConfigForm::onRobotParamLengthChange(double value)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_length = value;
    }
}

void RobotConfigForm::onRobotParamRotationChange(int value)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
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

QString RobotConfigForm::stringFromType(PART_TYPE tp)
{
    QString type;
    switch(tp)
    {
    case PART_TYPE::CORE_COMPONENT:
        type = "CoreComponent";
        break;
    case PART_TYPE::FIXED_BRICK:
        type = "FixedBrick";
        break;
    case PART_TYPE::ACTIVE_HINGE:
        type = "ActiveHinge";
        break;
    case PART_TYPE::PASSIVE_HINGE:
        type = "PassiveHinge";
        break;
    case PART_TYPE::PARAMETRIC_JOINT:
        type = "ParametricJoint";
        break;
    case PART_TYPE::LIGHT_SENSOR:
        type = "LightSensor";
        break;
    case PART_TYPE::IR_SENSOR:
        type = "IrSensor";
        break;
    case PART_TYPE::ACTIVE_WHEEL:
        type = "ActiveWheel";
        break;
    case PART_TYPE::PASSIVE_WHEEL:
        type = "PassiveWheel";
        break;
    }
    return type;
}

RobotConfigForm::~RobotConfigForm()
{
    delete ui;
}
