#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#include <QTreeWidget>
#include <QJsonObject>
#include <map>

enum class ItemClass {
    ROBOT, SIMULATION, EVOLUTION, PROJECT
};

class ConfigurationItem: public QTreeWidgetItem {
public:
    virtual ItemClass getType() = 0;
};


class RobotConfiguration: public ConfigurationItem {
public:
    virtual ItemClass getType();
    RobotConfiguration(QObject* parent = nullptr);
};

class SimulationConfiguration: public ConfigurationItem {
 public:
    virtual ItemClass getType();
    SimulationConfiguration(QObject* parent = nullptr);

    std::map<QString, QString> combo_map;
    std::map<QString, int> spin_map;
    std::map<QString, double> double_spin_map;
    std::map<QString, bool> bool_map;
};

class EvolutionConfiguration: public ConfigurationItem {
public:
    virtual ItemClass getType();
    EvolutionConfiguration(QObject* parent = nullptr);

    std::map<QString, QString> combo_map;
    std::map<QString, int> spin_map;
    std::map<QString, double> double_spin_map;
    std::map<QString, bool> bool_map;
};


class ProjectConfiguration: public ConfigurationItem {
public:
    virtual ItemClass getType();
    ProjectConfiguration(QObject* parent = nullptr);
    RobotConfiguration* robot = nullptr;
    SimulationConfiguration* simulation = nullptr;
    EvolutionConfiguration* evolution = nullptr;
};


#endif // CONFIGURATIONS_H
