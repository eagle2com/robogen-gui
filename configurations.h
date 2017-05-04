#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#include <QTreeWidget>
#include <QJsonObject>
#include <map>
#include <QListWidget>
#include <qcustomplot.h>

enum class PART_TYPE {CORE_COMPONENT, FIXED_BRICK, ACTIVE_HINGE, PASSIVE_HINGE, PARAMETRIC_JOINT, LIGHT_SENSOR, IR_SENSOR, ACTIVE_WHEEL, PASSIVE_WHEEL};
enum class PART_FACE {FRONT, BACK, RIGHT, LEFT};

class RobotPart: public QTreeWidgetItem
{
public:
    QString name = "";
    PART_TYPE type = PART_TYPE::FIXED_BRICK;
    PART_FACE face = PART_FACE::FRONT;
    int rotation = 0;
    int    param_rotation = 0;
    double param_length = 0.02;

    RobotPart(): QTreeWidgetItem()
    {
        this->update();

    }

    void update()
    {
        QString face_str;
        switch (face) {
        case PART_FACE::FRONT:
            face_str = " @FRONT";
            break;
        case PART_FACE::BACK:
            face_str = " @BACK";
            break;
        case PART_FACE::RIGHT:
            face_str = " @RIGHT";
            break;
        case PART_FACE::LEFT:
            face_str = " @LEFT";
            break;
        default:
            break;
        }

        QIcon icon;
        switch (type) {
        case PART_TYPE::CORE_COMPONENT:
            icon = QIcon(":/imgs/CoreComponent");
            break;
        case PART_TYPE::FIXED_BRICK:
            icon = QIcon(":/imgs/FixedBrick");
            break;
        case PART_TYPE::ACTIVE_HINGE:
            icon = QIcon(":/imgs/ActiveHinge");
            break;
        case PART_TYPE::PASSIVE_HINGE:
            icon = QIcon(":/imgs/PassiveHinge");
            break;
        case PART_TYPE::PARAMETRIC_JOINT:
            icon = QIcon(":/imgs/ParametricJoint");
            break;
        case PART_TYPE::LIGHT_SENSOR:
            icon = QIcon(":/imgs/LightSensor");
            break;
        case PART_TYPE::IR_SENSOR:
            icon = QIcon(":/imgs/IrSensor");
            break;
        case PART_TYPE::ACTIVE_WHEEL:
            icon = QIcon(":/imgs/ActiveWheel");
            break;
        case PART_TYPE::PASSIVE_WHEEL:
            icon = QIcon(":/imgs/PassiveWheel");
            break;
        default:
            break;
        }

        QString final_name = "";

        if(name == "")
            final_name = "<NONAME>";
        else
            final_name = name;

        QString rot_str;
        switch(rotation)
        {
        case 0:
            rot_str = " +0°";
            break;
        case 1:
            rot_str = " +90°";
            break;
        case 2:
            rot_str = " +180°";
            break;
        case 3:
            rot_str = " +270°";
            break;
        }

        this->setText(0, final_name + face_str + rot_str);
        this->setIcon(0, icon);

    }
};


class RunTreeItem: public QListWidgetItem {
public:
    QString path;
};


class RobotConfiguration {
public:
    RobotConfiguration(QObject* parent = nullptr);

    RobotPart* root_part = nullptr;
    QHash<QString, RobotPart*> robot_part_hash;
    QJsonObject get_json();
    bool loadRobot(const QString& filename = "");
    bool loadRobotJson(const QString& filename, QWidget *parent = nullptr);
    bool loadRobotFromObject(const QJsonObject& obj);
    std::map<QString, QString> text_map;
    std::map<QString, bool> bool_map;
};

class SimulationConfiguration {
 public:
    SimulationConfiguration(QObject* parent = nullptr);

    std::map<QString, QString> combo_map;
    std::map<QString, int> spin_map;
    std::map<QString, double> double_spin_map;
    std::map<QString, bool> bool_map;
    std::map<QString, QString> text_map;
};

class EvolutionConfiguration {
public:
    EvolutionConfiguration(QObject* parent = nullptr);

    std::map<QString, QString> combo_map;
    std::map<QString, int> spin_map;
    std::map<QString, double> double_spin_map;
    std::map<QString, bool> bool_map;
    std::map<QString, QString> text_map;
};


class ProjectConfiguration: public QListWidgetItem {
public:
    ProjectConfiguration(QObject* parent = nullptr);
    RobotConfiguration* robot = nullptr;
    SimulationConfiguration* simulation = nullptr;
    EvolutionConfiguration* evolution = nullptr;
    QString root_directory = "";
    QList<RunTreeItem*> run_list;
};


#endif // CONFIGURATIONS_H
