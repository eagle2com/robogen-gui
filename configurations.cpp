#include "configurations.h"

ItemClass ProjectConfiguration::getType() {
    return ItemClass::PROJECT;
}

ProjectConfiguration::ProjectConfiguration(QObject *parent)
{
    robot = new RobotConfiguration();
    simulation = new SimulationConfiguration();
    evolution = new EvolutionConfiguration();

    this->addChild(robot);
    this->addChild(simulation);
    this->addChild(evolution);
}

ItemClass RobotConfiguration::getType() {
    return ItemClass::ROBOT;
}

RobotConfiguration::RobotConfiguration(QObject *parent)
{
    setText(0, "Robot");
}

ItemClass SimulationConfiguration::getType() {
    return ItemClass::SIMULATION;
}

SimulationConfiguration::SimulationConfiguration(QObject *parent)
{
    setText(0, "Simulation");
}

ItemClass EvolutionConfiguration::getType() {
    return ItemClass::EVOLUTION;
}

EvolutionConfiguration::EvolutionConfiguration(QObject *parent)
{
    setText(0, "Evolution");
}
