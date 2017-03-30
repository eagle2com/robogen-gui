#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <qcustomplot.h>

#include <stdio.h>
#include <iostream>

using std::cout;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    simulation_config_form = new SimulationConfigForm();
    robot_config_form = new RobotConfigForm();
    evolution_config_form = new EvolutionConfigForm();
    overview_form = new ConfigOverviewForm();
    settings_window = new SettingsWindow(this);
    settings_window->hide();

    ui->main_frame->setLayout(new QGridLayout());

    ui->main_frame->layout()->addWidget(simulation_config_form);
    ui->main_frame->layout()->addWidget(robot_config_form);
    ui->main_frame->layout()->addWidget(evolution_config_form);
    ui->main_frame->layout()->addWidget(overview_form);

    simulation_config_form->hide();
    evolution_config_form->hide();
    robot_config_form->hide();

    connect(ui->tree_project, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onProjectTreeSelect(QTreeWidgetItem*,int)));
    connect(ui->tree_project, SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(onProjectTreeSelect(QTreeWidgetItem*,int)));
    connect(ui->tree_project, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onProjectTreeSelect(QTreeWidgetItem*,int)));

    connect(ui->push_add, SIGNAL(pressed()), this, SLOT(onProjectTreeAdd()));
    connect(ui->push_remove, SIGNAL(pressed()), this, SLOT(onProjectTreeRemove()));

    connect(ui->action_new_project, SIGNAL(triggered()), this, SLOT(onNewProject()));
    connect(ui->action_open_project, SIGNAL(triggered()), this, SLOT(onOpenProject()));
    connect(ui->action_save_project, SIGNAL(triggered()), this, SLOT(onSaveProject()));

    connect(ui->action_settings, SIGNAL(triggered()), settings_window, SLOT(show()));


    /*

    connect(ui->robotConfigTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->action_loadsimulation, SIGNAL(triggered(bool)), this, SLOT(loadSimulation()));
    connect(ui->action_loadevolution, SIGNAL(triggered(bool)), this, SLOT(loadEvolution()));
    connect(ui->action_loadrobot, SIGNAL(triggered(bool)), this, SLOT(loadRobot()));

    //connect(ui->action_savesimulation, SIGNAL(triggered(bool)), this, SLOT(saveSimulation()));
    //connect(ui->action_saveevolution, SIGNAL(triggered(bool)), this, SLOT(saveEvolution()));
    //connect(ui->action_saverobot, SIGNAL(triggered(bool)), this, SLOT(saveRobot()));

    connect(ui->action_saveall, SIGNAL(clicked(bool)), this, SLOT(saveAll()));

    connect(ui->push_robogenbrowse, SIGNAL(clicked(bool)), this, SLOT(browseRobogen()));
    connect(ui->push_browseevolve, SIGNAL(clicked(bool)), this, SLOT(browseEvolve()));
    connect(ui->push_browsescenario, SIGNAL(clicked(bool)), this, SLOT(browseCustomScenario()));

    connect(ui->push_evolve, SIGNAL(clicked(bool)), this, SLOT(onPushEvolve()));
    connect(ui->push_simulate, SIGNAL(clicked(bool)), this, SLOT(onPushSimulate()));
    connect(ui->push_analyze, SIGNAL(clicked(bool)), this, SLOT(onPushAnalyze()));

    connect(ui->action_loadconfig, SIGNAL(triggered(bool)), this, SLOT(onLoadConfig()));
    connect(ui->action_saveconfig, SIGNAL(triggered(bool)), this, SLOT(onSaveConfig()));
    connect(ui->action_saveconfig_as, SIGNAL(triggered(bool)), this, SLOT(onSaveConfigAs()));

    connect(ui->push_stop, SIGNAL(clicked(bool)), this, SLOT(onPushStop()));

    connect(&dir_watcher, SIGNAL(onNewFile(QStringList)), this, SLOT(onNewFileList(QStringList)));
*/


    qsrand(QTime::currentTime().elapsed());

    //loadConfig("./config.json");

    onNewProject();
}





void MainWindow::loadEvolution()
{
    /*
    QString filename = QFileDialog::getOpenFileName(this, "", evolution_filename);
    if(filename != "")
    {
        evolution_filename = filename;
    }
    */
}

void MainWindow::loadRobot()
{
    /*
    QString filename = robot_config_filename;
    if(filename == "") {
        filename = QFileDialog::getOpenFileName(this, "Open robot", project_path, "*.txt *.json");
        if(filename == "")
            return;
    }

    robot_config_filename = filename;

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

        ui->robotConfigTree->clear();
        robot_part_hash.clear();

        if(root_part) delete root_part;
        root_part = new RobotPart;
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
            qDebug() << tokens;
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
            robot_part_hash.insert(new_part->name, new_part);
            root_part->update();

            prev_part = new_part;
        }

        ui->robotConfigTree->addTopLevelItem(root_part);
        ui->robotConfigTree->expandAll();
    }
    else
    {
        qDebug() << "FAILED TO OPEN ROBOT FILE: " << filename <<endl;
    }
    */
}

void MainWindow::loadRobotJson(QString filename)
{
    /*
    qDebug() << "Opening robot json config: " << filename << " ... ";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[FAIL]" << endl;
        return;
    }
    qDebug() << "[ OK ]";

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = doc.object();

    QJsonArray parts = object["body"].toObject()["part"].toArray();

    qDebug() << "Found " << parts.size() << " parts";

    QHash<QString, RobotPart*> hash;

    RobotPart *new_root_part = new RobotPart;
    foreach (const QJsonValue & part, parts)
    {
        RobotPart *new_part = new RobotPart;
        new_part->name = part.toObject()["id"].toString();
        new_part->type = typeFromString(part.toObject()["type"].toString());
        qDebug() << "type string: " << part.toObject()["type"].toString();
        new_part->rotation = part.toObject()["orientation"].toInt();
        if(new_part->type == PART_TYPE::CORE_COMPONENT)
            new_root_part = new_part;

        qDebug() << "part: NAME='" << new_part->name << "' type='" << (int)new_part->type << "' rotation='" << new_part->rotation;
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
    qDebug() << "Root part:" << new_root_part->name << endl;
    root_part = new_root_part;
    root_part->update();
    ui->robotConfigTree->clear();
    ui->robotConfigTree->addTopLevelItem(root_part);
    ui->robotConfigTree->expandAll();
    */
}

void MainWindow::onLoadConfig()
{
    //loadConfig();
}

void MainWindow::loadConfig(QString default_path)
{
    /*
    QString filename;
    if(default_path != "")
    {
        filename = default_path;
        //qDebug() << "Found default config.json file, loading ..." << endl;
    }
    else
    {
        filename = QFileDialog::getOpenFileName(this, "");
    }

    if(filename == "")
            return;

    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        if(filename == default_path)
        {
            qDebug() << "Found default config.json file, loading ..." << endl;
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject object = doc.object();
        qDebug() << "build path:" << object["build-path"].toString() << endl;
        if(object["build-path"].toString() != "")
        {
            ui->edit_robogenpath->setText(object["build-path"].toString());
        }

        qDebug() << "output path:" << object["output-path"].toString() << endl;
        if(object["output-path"].toString() != "")
        {
            ui->line_evolve->setText(object["output-path"].toString());
        }

        qDebug() << "config path:" << object["config-path"].toString() << endl;
        if(object["config-path"].toString() != "")
        {
            project_path = object["config-path"].toString();
        }

        if(object["simulation-config-file"].toString() != "") {
            simulation_config_filename = object["simulation-config-file"].toString();
            loadSimulation();
        }

        if(object["robot-config-file"].toString() != "") {
            robot_config_filename = object["robot-config-file"].toString();
            loadRobot();
        }
    }
    */
}

void MainWindow::saveConfig()
{
    /*
    QString filename = config_filename;
    if(filename == "")
            return;

    QFile file(filename);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open " + filename + " for saving");
        return;
    }

    QJsonDocument doc = QJsonDocument();
    QJsonObject object;
    object["build-path"] = ui->edit_robogenpath->text();
    object["output-path"] = ui->line_evolve->text();
    //object["config-path"] = ui->edit_robogenpath->text();
    object["simulation-config-file"] = simulation_config_filename;
    object["evolution-config-file"] = evolution_config_filename;
    object["robot-config-file"] = robot_config_filename;

    doc.setObject(object);
    file.write(doc.toJson());
*/
}

void MainWindow::onSaveConfig()
{
    /*
    if(config_filename == "")
        onSaveConfigAs();
    else
        saveConfig();
        */
}

void MainWindow::onSaveConfigAs()
{
    /*
    config_filename = QFileDialog::getSaveFileName(this, "Save File", config_filename, "*.json");
    saveConfig();
    */
}

void MainWindow::saveSimulation()
{
    /*
    QString filename = project_path + "/sim.txt";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream << "# GENERATED BY ROBOGEN GUI" << endl;
        if(ui->check_customscenario->isChecked())
            stream <<"scenario=" << ui->line_scenario->text() << endl;
        else
            stream <<"scenario=" << ui->combo_scenario->currentText() << endl;
        stream <<"timeStep=" << ui->spin_timestep->value() << endl;
        stream <<"actuationFrequency="<< ui->spin_actuationfrequency->value() << endl;
        stream <<"nTimeSteps="<< ui->spin_ntimesteps->value() << endl;
        stream <<"terrainType="<< ui->combo_terraintype->currentText() << endl;
        stream <<"terrainLength="<< ui->spin_terrainlength->value() << endl;
        stream <<"terrainWidth="<< ui->spin_terrainwidth->value() << endl;
        stream <<"terrainFriction="<< ui->spin_terrainfriction->value() << endl;
        stream <<"sensorNoiseLevel="<< ui->spin_sensornoiselevel->value() << endl;
        stream <<"motorNoiseLevel="<< ui->spin_motornoiselevel->value() << endl;
        stream <<"capAcceleration="<< (ui->check_capacceleration->isChecked() ? "true": "false") << endl;
        if(ui->line_obstacles->text().length() > 0) {
            stream <<"obstaclesConfigFile="<< ui->line_obstacles->text() << endl;
        }
    }
    else
    {
        qDebug() << "FAILED TO SAVE SIMULATION TO: " << filename << endl;
        QMessageBox::critical(this, "Error", "Failed to save simulation config to: " + filename);
    }
    */
}

void MainWindow::loadSimulation()
{
    /*
    QString filename = "";
    if(simulation_config_filename == "") {
        filename = QFileDialog::getOpenFileName(this, "", project_path);
        if(filename == "")
            return;

        simulation_config_filename = filename;
    } else {
        filename = simulation_config_filename;
    }

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open simulation config file: " + filename);
        simulation_config_filename = "";
        return;
    }

    QTextStream stream(&file);
    while(!stream.atEnd()) {
        QString line = stream.readLine();
        // Separate key/value with =
        QStringList tok = line.split("=");
        if(tok[0] == "scenario") ui->combo_scenario->setCurrentText(tok[1]);
        else if(tok[0] == "timeStep") ui->spin_timestep->setValue(tok[1].toDouble());
        else if(tok[0] == "actuationFrequency") ui->spin_actuationfrequency->setValue(tok[1].toDouble());
        else if(tok[0] == "nTimeSteps") ui->spin_ntimesteps->setValue(tok[1].toInt());
        else if(tok[0] == "terrainType") ui->combo_terraintype->setCurrentText(tok[1]);
        else if(tok[0] == "terrainLength") ui->spin_terrainlength->setValue(tok[1].toDouble());
        else if(tok[0] == "terrainFriction") ui->spin_terrainfriction->setValue(tok[1].toDouble());
        else if(tok[0] == "sensorNoiseLevel") ui->spin_sensornoiselevel->setValue(tok[1].toDouble());
        else if(tok[0] == "motorNoiseLevel") ui->spin_motornoiselevel->setValue(tok[1].toDouble());
        else if(tok[0] == "capAcceleration") ui->check_capacceleration->setChecked(tok[1]=="true"? true : false);
        else if(tok[0] == "obstaclesConfigFile") ui->line_obstacles->setText(tok[1]);
    }
    */
}


void MainWindow::saveEvolution()
{
    /*
    QString filename = project_path + "/evo.txt";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream << "# GENERATED BY ROBOGEN GUI" << endl;
        stream <<"simulatorConfFile=" << project_path + "/sim.txt" << endl;
        stream <<"referenceRobotFile=" << project_path + "/robot.txt" << endl;
        stream <<"mu=" << ui->spin_mu->value() << endl;
        stream <<"lambda=" << ui->spin_lambda->value() << endl;
        stream <<"selection="<< ui->combo_selection->currentText() << endl;
        stream <<"replacement="<< ui->combo_replacement->currentText()<< endl;
        stream <<"tournamentSize="<< ui->spin_tournament->value() << endl;
        stream <<"numGenerations="<< ui->spin_generations->value() << endl;
        stream <<"evolutionMode="<< ui->combo_evolutionmode->currentText() << endl;
        stream <<"pBrainMutate="<< ui->spin_pbrainmutate->value() << endl;
        stream <<"brainSigma="<< ui->spin_brainsigma->value() << endl;
        stream <<"pBrainCrossover="<< ui->spin_pbraincrossover->value() << endl;
        stream <<"addBodyPart="<< "All" << endl;                                        ///TODO: Add to config
        stream <<"pNodeInsert=" << ui->spin_pnodeinsert->value() << endl;
        stream <<"pSubtreeRemove=" << ui->spin_psubtreeremove->value() << endl;
        stream <<"pSubtreeDuplicate=" << ui->spin_psubtreeduplicate->value() << endl;
        stream <<"pSubtreeSwap=" << ui->spin_psubtreeswap->value() << endl;
        stream <<"pNodeRemove=" << ui->spin_pnoderemove->value() << endl;
        stream <<"pParameterModify=" << ui->spin_pparemetermodify->value() << endl;
        stream <<"brainBounds=-3:3" << endl;                                            ///TODO: Add to configurations
        stream <<"pAddHiddenNeuron=" << ui->spin_paddhiddenneuron->value() << endl;
        stream <<"maxBodyParts=" << ui->spin_maxbodyparts->value() << endl;
        if(ui->combo_evolutionmode->currentText() == "full" && ui->combo_evolutionaryalgorithm->currentText() == "HyperNEAT") {
            stream <<"evolutionaryAlgorithm=" << "Basic" << endl;    // Basic, HyperNEAT
            qDebug() << "WARNING: HyperNEAT cannot be used with full evolutions, setting to Basic" << endl;
            QMessageBox::warning(this, "Warning", "HyperNEAT canno be used with full evolutions, using Basic");
        }
        else {
            stream <<"evolutionaryAlgorithm=" << ui->combo_evolutionaryalgorithm->currentText() << endl;    // Basic, HyperNEAT
        }

        for(int i = 0; i < ui->spin_threads->value(); i++)
        {
            stream <<"socket=127.0.0.1:800"<<i+1<<endl;
        }
    }
    else
    {
        qDebug() << "FAILED TO SAVE EVOLUTION TO: " << filename << endl;
    }
    */
}

void writeRobotPart(RobotPart* part, int tab, QTextStream& stream)
{
    /*
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

    for(int i = 0; i < tab; i++)
    {
        stream << "\t";
    }
    name = part->name;
    if(name.isEmpty())
        name = "PARTGEN_" + QString::number((ulong)part);

    stream << (int)part->face <<" " << type <<" "<< name << " " << (int)part->rotation;
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
    */
}

void MainWindow::saveRobot()
{
    /*
    QString filename = project_path + "/robot.txt";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        writeRobotPart(root_part, 0, stream);
        //FIXME Dirty hack
        stream << "\n\n\nLeftWheel 0 100" << endl;
        stream << "RightWheel 0 -100" << endl;
    }
    else
    {
        qDebug() << "FAILED TO SAVE ROBOT TO: " << filename << endl;
    }
    */
}

void MainWindow::saveAll()
{
    /*
    if(project_path.isEmpty())
    {
        QString path = QFileDialog::getExistingDirectory(this);
        project_path = path;
    }

    if(!project_path.isEmpty())
    {
        saveRobot();
        saveEvolution();
        saveSimulation();
    }
    */
}

void MainWindow::browseRobogen()
{
    /*
    QString path = QFileDialog::getExistingDirectory(this);

    ui->edit_robogenpath->setText(path);
    */
}

void MainWindow::browseEvolve()
{
    /*
    QString path = QFileDialog::getExistingDirectory(this);

    ui->line_evolve->setText(path);
    */
}

void MainWindow::browseCustomScenario()
{
    /*
    QString filename = QFileDialog::getOpenFileName(this);
    ui->line_scenario->setText(filename);
    */
}

void MainWindow::onPushEvolve()
{   
    /*
    if(fs_watcher)
        delete fs_watcher;

    if(ui->line_evolve->text().isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Please select an output directory.");
        msgBox.exec();
        return;
    }
    QDir dir(ui->line_evolve->text());

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    if(ui->edit_robogenpath->text().isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Path to robogen build folder is not set.");
        msgBox.exec();
        return;
    }

    ui->push_evolve->setDisabled(true);

    // Make sure to kill any running servers to avoid conflicts
    ///FIXME: Maybe move the killing into a separate method and keep the callback just for the button press?
    onPushStop();

    // Launch the server(s)
    {
        QString program = ui->edit_robogenpath->text() + "/robogen-server";
        QStringList arguments;
        for(int i = 0; i < ui->spin_threads->value()-1; i++)
        {
            arguments.clear();
            arguments << QString::number(8000 + i + 1);
            QProcess *process = new QProcess(this);
            process->setProcessChannelMode(QProcess::ForwardedChannels);
            process->start(program, arguments);
            process_list.push_back(process);

            //qDebug() << "Launching server #" << i + 1 << endl;;
        }

        arguments.clear();
        arguments << QString::number(8000 + ui->spin_threads->value());
        QProcess *process = new QProcess(this);
        process->setProcessChannelMode(QProcess::ForwardedChannels);
        process->start(program, arguments);
        process_list.push_back(process);

        //qDebug() << "Launching server #" << ui->spin_threads->value() << endl;
    }

    // Launch an evolution
    {
        QString program = ui->edit_robogenpath->text() + "/robogen-evolver";
        QStringList arguments;
        QString seed;
        if(ui->check_randomseed->isChecked())
            seed = QString::number(qrand());
        else
            seed = QString::number(ui->spin_seed->value());
        arguments << seed << ui->line_evolve->text() << project_path + "/evo.txt" << "--overwrite";
        qDebug() << "arguments: " << arguments << endl;

        n_generation = 0;
        ui->list_generationsbest->clear();
        ui->progress_generation->setMaximum(ui->spin_generations->value());
        ui->progress_generation->setValue(0);
        process_evolve = new QProcess(this);
        process_evolve->setProcessChannelMode(QProcess::ForwardedErrorChannel);
        process_evolve->start(program, arguments);

        connect(process_evolve, SIGNAL(finished(int)), this, SLOT(onEvolveFinished(int)));
        process_evolve->waitForStarted();
    }

    // Create file system watcher so we can check for new generations
    //{
    //    fs_watcher = new QFileSystemWatcher;
    //    fs_watcher->addPath(ui->line_evolve->text() + "/../");
    //    connect(fs_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onFileChanged(QString)));
    //    qDebug() << "watching " << fs_watcher->directories() << endl;
   // }

    dir_watcher.watch(ui->line_evolve->text(),500);
    */
}

void MainWindow::onPushSimulate()
{
    /*
    QString program = ui->edit_robogenpath->text() + "/robogen-file-viewer";
    QStringList arguments;
    QListWidgetItem *current = ui->list_generationsbest->currentItem();
    if(current)
    {
        QString path = ui->line_evolve->text() + "/" + current->text();
        arguments << path << project_path + "/sim.txt";
        process_simulate = new QProcess(this);
        process_simulate->setWorkingDirectory(ui->edit_robogenpath->text());
        process_simulate->start(program, arguments);
    }
    */
}

void MainWindow::onPushAnalyze()
{
    /*
    QString path = ui->line_evolve->text() + "/BestAvgStd.txt";
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream in(&file);

    QVector<double> v_best, v_avg, v_std_up, v_std_down;
    QVector<double> v_x;

    while(!in.atEnd())
    {
        int x;
        double best, avg, std;
        in >> x >> best >> avg >> std;
        v_best.push_back(best);
        v_avg.push_back(avg);
        v_std_up.push_back(avg + std);
        v_std_down.push_back(avg - std);
        v_x.push_back(x);
    }

    QCustomPlot *customPlot = new QCustomPlot;
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
    customPlot->legend->setTextColor(Qt::white);
    customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 20)));

    QCPGraph *graph1 = customPlot->addGraph();
    graph1->setData(v_x, v_avg);
   // graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    graph1->setPen(QPen(QColor(255, 255, 255), 2));
    graph1->setName("Average");

    QCPGraph *graph2 = customPlot->addGraph();
    graph2->setData(v_x, v_std_up);
    graph2->setPen(QPen(QColor(0,0,255,64),2));
    graph2->setBrush(QColor(200, 200, 200, 20));
    graph2->setChannelFillGraph(graph1);
    graph2->setName("STD");

    QCPGraph *graph3 = customPlot->addGraph();
    graph3->setData(v_x, v_std_down);
    graph3->setPen(QPen(QColor(0,0,255,64),2));
    graph3->setBrush(QColor(200, 200, 200, 20));
    graph3->setChannelFillGraph(graph1);
    graph3->setName("STD");

    QCPGraph *graph4 = customPlot->addGraph();
    graph4->setData(v_x, v_best);
   // graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    graph4->setPen(QPen(QColor(20, 200, 20), 2));
    graph4->setName("Best");

    // set some pens, brushes and backgrounds:
    customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);


    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    customPlot->setBackground(plotGradient);

    customPlot->rescaleAxes();
    customPlot->replot();
    customPlot->resize(800,600);
    customPlot->setWindowTitle("Robogen Generations Plot");
    customPlot->show();

    qDebug() << "PLOTTING" << v_best.length();
    */
}

void MainWindow::onPushStop()
{
    /*
    qDebug() << "Killing all servers ..." << endl;
    for(QProcess* p: process_list)
    {
        p->kill();
    }

    process_list.clear();
    */
}

void MainWindow::onFileChanged(QString filename)
{
    /*
    ui->list_generationsbest->addItem(filename);
    if(filename.startsWith("Generation"))
    {
        ui->list_generationsbest->addItem(filename);
    }
    */
}

void MainWindow::onNewFileList(QStringList files)
{
    /*
   // qDebug() << files << endl;
    auto index = ui->list_generationsbest->currentIndex();
    ui->list_generationsbest->clear();
    ui->list_generationsbest->addItems(files);
    ui->list_generationsbest->setCurrentIndex(index);
    */
}

void MainWindow::evolveReadReady()
{
    /*
    QByteArray data = process_evolve->readLine();
    QString line(data);
    qDebug() << line;
    if(line.contains("Generation"))
    {
        n_generation++;
        if(n_generation % 2 == 0)
        {
            ui->progress_generation->setValue(n_generation/2);
            ui->list_generationsbest->addItem(QString("GenerationBest-") + QString::number(n_generation/2) + ".json");
        }
    }
    */
}

void MainWindow::onEvolveFinished(int)
{
    // ui->push_evolve->setEnabled(true);
}

void MainWindow::onFileEvent(const QString& str)
{

}

void MainWindow::onNewProject()
{
    ui->tree_project->clear();
    onProjectTreeAdd();
    simulation_config_form->current_config = nullptr;
    simulation_config_form->hide();

    evolution_config_form->current_config = nullptr;
    evolution_config_form->hide();

    robot_config_form->hide();
    overview_form->show();
    overview_form->current_config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(0));
    overview_form->loadAll();

    ui->tree_project->setCurrentItem(ui->tree_project->topLevelItem(0));
}

void MainWindow::onOpenProject()
{
    //TODO Check if this project is saved?
    project_directory = QFileDialog::getExistingDirectory(this, "Choose project directory to open", "");
    if(project_directory == "")
        return;

    QFile project_file(project_directory + "/project.json");
    if(!project_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Saving Error", "Could not open " + project_directory + " for reading\nProject open failed");
        return;
    }

    ui->tree_project->clear();

    QJsonDocument doc = QJsonDocument::fromJson(project_file.readAll());
    QJsonObject main_object = doc.object();
    settings_window->set_robogen_directory(main_object["robogen_directory"].toString());

    QJsonArray configs = main_object["configurations"].toArray();
    for(int i = 0; i < configs.size(); i++) {
        QJsonObject config = configs[i].toObject();
        ProjectConfiguration* project1_config = new ProjectConfiguration();
        project1_config->setText(0, config["name"].toString());
        ui->tree_project->addTopLevelItem(project1_config);

        QJsonObject simulation_obj = config["simulation"].toObject();
        QJsonObject evolution_obj = config["evolution"].toObject();

        for(const QString& key: simulation_obj.keys()) {
            if(key.startsWith("combo")) {
                project1_config->simulation->combo_map[key] = simulation_obj.value(key).toString();
            }
            else if(key.startsWith("spin")) {
                project1_config->simulation->spin_map[key] = simulation_obj.value(key).toInt();
            }
            else if(key.startsWith("dspin")) {
                project1_config->simulation->double_spin_map[key] = simulation_obj.value(key).toDouble();
            }
        }

        for(const QString& key: evolution_obj.keys()) {
            if(key.startsWith("combo")) {
                project1_config->evolution->combo_map[key] = evolution_obj.value(key).toString();
            }
            else if(key.startsWith("spin")) {
                project1_config->evolution->spin_map[key] = evolution_obj.value(key).toInt();
            }
            else if(key.startsWith("dspin")) {
                project1_config->evolution->double_spin_map[key] = evolution_obj.value(key).toDouble();
            }
        }
    }

    simulation_config_form->current_config = nullptr;
    simulation_config_form->hide();

    evolution_config_form->current_config = nullptr;
    evolution_config_form->hide();

    robot_config_form->hide();
    overview_form->show();
    overview_form->current_config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(0));
    overview_form->loadAll();

    ui->tree_project->setCurrentItem(ui->tree_project->topLevelItem(0));
}

void MainWindow::onSaveProject()
{
    if(project_directory == "") {
        project_directory = QFileDialog::getExistingDirectory(this, "Choose project directory", "");
        if(project_directory == "")
            return;
    }

    QFile project_file(project_directory + "/project.json");
    if(!project_file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        QMessageBox::critical(this, "Saving Error", "Could not open " + project_directory + " for writing\nProject save failed");
        return;
    }

    if(!simulation_config_form->isHidden()) simulation_config_form->saveAll();
    if(!evolution_config_form->isHidden()) evolution_config_form->saveAll();

    QJsonDocument doc;
    QJsonObject main_object;

    main_object["robogen_directory"] = settings_window->get_robogen_directory();

    QJsonArray config_array;
    for(int i = 0; i < ui->tree_project->topLevelItemCount(); i++) {
        ProjectConfiguration* config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(i));
        QJsonObject config_obj;
        QJsonObject evolution_obj;
        for(auto& pair: config->evolution->combo_map) {
            evolution_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->evolution->spin_map) {
            evolution_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->evolution->double_spin_map) {
            evolution_obj[pair.first] = pair.second;
        }
        config_obj["evolution"] = evolution_obj;

        QJsonObject simulation_obj;
        for(auto& pair: config->simulation->combo_map) {
            simulation_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->simulation->spin_map) {
            simulation_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->simulation->double_spin_map) {
            simulation_obj[pair.first] = pair.second;
        }
        config_obj["simulation"] = simulation_obj;
        config_obj["name"] = config->text(0);
        config_array.push_back(config_obj);
    }

    main_object["configurations"] = config_array;

    doc.setObject(main_object);

    project_file.write(doc.toJson());

    //TODO Save robot configuration
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onProjectTreeSelect(QTreeWidgetItem *item_, int)
{
    //if(!robot_config_form->isHidden()) robot_config_form->saveAll();
    if(!simulation_config_form->isHidden()) simulation_config_form->saveAll();
    if(!evolution_config_form->isHidden()) evolution_config_form->saveAll();
    ConfigurationItem* item = dynamic_cast<ConfigurationItem*>(item_);
    //qDebug() << "select: " << (int)item->getType();


    robot_config_form->hide();
    simulation_config_form->hide();
    evolution_config_form->hide();
    overview_form->hide();

    switch (item->getType()) {
    case ItemClass::EVOLUTION:
        evolution_config_form->show();
        evolution_config_form->current_config = dynamic_cast<EvolutionConfiguration*>(item);
        evolution_config_form->loadAll();
        break;
    case ItemClass::ROBOT:
        robot_config_form->show();
        break;
    case ItemClass::SIMULATION:
        simulation_config_form->show();
        simulation_config_form->current_config = dynamic_cast<SimulationConfiguration*>(item);
        simulation_config_form->loadAll();
        break;
    case ItemClass::PROJECT:
        overview_form->show();
        overview_form->current_config = dynamic_cast<ProjectConfiguration*>(item);
        overview_form->loadAll();
        break;
    default:
        break;
    }

}

void MainWindow::onProjectTreeAdd()
{
    ProjectConfiguration* project1_config = new ProjectConfiguration();

    project1_config->setText(0, "Configuration 1");
    ui->tree_project->addTopLevelItem(project1_config);

    EvolutionConfiguration* evo_config = evolution_config_form->current_config;
    evolution_config_form->current_config = project1_config->evolution;
    evolution_config_form->saveAll();
    evolution_config_form->current_config = evo_config;

    SimulationConfiguration* sim_config = simulation_config_form->current_config;
    simulation_config_form->current_config = project1_config->simulation;
    simulation_config_form->saveAll();
    simulation_config_form->current_config = sim_config;
}

void MainWindow::onProjectTreeRemove()
{
    if(ui->tree_project->topLevelItemCount() < 2)
        return;
    ConfigurationItem* item = dynamic_cast<ConfigurationItem*>(ui->tree_project->currentItem());
    if(item != nullptr && item->getType() == ItemClass::PROJECT) {
        delete item;
    }
}

