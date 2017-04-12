#include "configurations.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

#include "robotconfigform.h"


ProjectConfiguration::ProjectConfiguration(QObject *parent)
{
    robot = new RobotConfiguration();
    simulation = new SimulationConfiguration();
    evolution = new EvolutionConfiguration();
}


RobotConfiguration::RobotConfiguration(QObject *parent)
{
    root_part = new RobotPart();
    root_part->type = PART_TYPE::CORE_COMPONENT;
    root_part->name = "ROOT";
}

QJsonObject RobotConfiguration::get_json()
{
    QJsonObject obj;

    QJsonObject body_obj;

    // Array of parts
    QJsonArray part_array;

    // Array of connections
    QJsonArray conn_array;

    QTreeWidget tmp_tree;
    tmp_tree.addTopLevelItem(root_part);
    QTreeWidgetItemIterator it(root_part);

    while(*it) {
        RobotPart* part = dynamic_cast<RobotPart*>(*it);
        QJsonObject part_obj;

        if(part->name.isEmpty()) part->name = "PARTGEN_" + QString::number((ulong)part);
        part_obj["id"] = part->name;
        part_obj["type"] = RobotConfigForm::stringFromType(part->type); //TODO move this elsewhere
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

    tmp_tree.takeTopLevelItem(0);

    root_part->update();

    return obj;
}

bool RobotConfiguration::loadRobot(const QString &filename)
{
    QString extension;
    {
        QStringList tokens = filename.split('.');
        extension = tokens.back();
    }

    if(extension == "json")
    {
        return loadRobotJson(filename);
    }

    QFile file(filename);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line;

        robot_part_hash.clear();

        if(root_part) delete root_part;
        root_part = new RobotPart();
        root_part->type = PART_TYPE::CORE_COMPONENT;

        in.readLine();
        RobotPart* current_parent = root_part;
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

            new_part = new RobotPart();

            new_part->face = (PART_FACE)tokens.front().toInt(); tokens.pop_front();
            QString type = tokens.front(); tokens.pop_front();

            new_part->type = RobotConfigForm::typeFromString(type);
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
            robot_part_hash.insert(new_part->name, new_part);
            root_part->update();

            prev_part = new_part;
        }

        //ui->robotConfigTree->addTopLevelItem(current_config->root_part);
        //ui->robotConfigTree->expandAll();
        //TODO Call this
    }
    else
    {
        qDebug() << "FAILED TO OPEN ROBOT FILE: " << filename <<endl;
    }

    return true;
}

bool RobotConfiguration::loadRobotJson(const QString &filename, QWidget* parent)
{
    //qDebug() << "Opening robot json config: " << filename << " ... ";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        //qDebug() << "[FAIL]" << endl;
        QMessageBox::critical(parent, "Error", "Could not open " + filename + " for reading.");
        return false;
    }
    //qDebug() << "[ OK ]";

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = doc.object();

    QJsonArray parts = object["body"].toObject()["part"].toArray();

    //qDebug() << "Found " << parts.size() << " parts";

    QHash<QString, RobotPart*> hash;

    if(root_part) delete root_part;

    foreach (const QJsonValue & part, parts)
    {
        RobotPart *new_part = new RobotPart;
        new_part->name = part.toObject()["id"].toString();
        new_part->type = RobotConfigForm::typeFromString(part.toObject()["type"].toString());
        //qDebug() << "type string: " << part.toObject()["type"].toString();
        new_part->rotation = part.toObject()["orientation"].toInt();
        if(new_part->type == PART_TYPE::CORE_COMPONENT)
            root_part = new_part;

        //qDebug() << "part: NAME='" << new_part->name << "' type='" << (int)new_part->type << "' rotation='" << new_part->rotation;
        hash.insert(new_part->name, new_part);
    }

    QJsonArray connections = object["body"].toObject()["connection"].toArray();
    foreach (const QJsonValue & connection, connections)
    {
        RobotPart *parent = hash[connection.toObject()["src"].toString()];
        RobotPart *child = hash[connection.toObject()["dest"].toString()];
        child->face = (PART_FACE)connection.toObject()["srcSlot"].toInt();
        // FIXME Check srcSlot destSlot combinations
        if(child->face == PART_FACE::BACK) child->face = PART_FACE::FRONT;
        else if(child->face == PART_FACE::FRONT) child->face = PART_FACE::BACK;
        parent->addChild(child);
        child->update();
    }

    root_part->update();
    return true;
}

bool RobotConfiguration::loadRobotFromObject(const QJsonObject &obj)
{
    QJsonArray parts = obj["body"].toObject()["part"].toArray();

    //qDebug() << "Found " << parts.size() << " parts";

    QHash<QString, RobotPart*> hash;

    if(root_part) delete root_part;

    foreach (const QJsonValue & part, parts)
    {
        RobotPart *new_part = new RobotPart;
        new_part->name = part.toObject()["id"].toString();
        new_part->type = RobotConfigForm::typeFromString(part.toObject()["type"].toString());
        //qDebug() << "type string: " << part.toObject()["type"].toString();
        new_part->rotation = part.toObject()["orientation"].toInt();
        if(new_part->type == PART_TYPE::CORE_COMPONENT)
            root_part = new_part;

        //qDebug() << "part: NAME='" << new_part->name << "' type='" << (int)new_part->type << "' rotation='" << new_part->rotation;
        hash.insert(new_part->name, new_part);
    }

    QJsonArray connections = obj["body"].toObject()["connection"].toArray();
    foreach (const QJsonValue & connection, connections)
    {
        RobotPart *parent = hash[connection.toObject()["src"].toString()];
        RobotPart *child = hash[connection.toObject()["dest"].toString()];
        child->face = (PART_FACE)connection.toObject()["srcSlot"].toInt();
        parent->addChild(child);
        child->update();
    }

    root_part->update();
    return true;
}


SimulationConfiguration::SimulationConfiguration(QObject *parent)
{

}


EvolutionConfiguration::EvolutionConfiguration(QObject *parent)
{

}
