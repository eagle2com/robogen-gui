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
#include <QStringList>
#include <qcustomplot.h>

#include <stdio.h>
#include <iostream>

#include <chrono>
#include <thread>

using namespace std::chrono;

using std::cout;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings_window = new SettingsWindow(this);
    settings_window->hide();

    connect(ui->tree_project, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onProjectTreeSelect(QListWidgetItem*)));
    //connect(ui->tree_project, SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(onProjectTreeSelect(QTreeWidgetItem*,int)));
    //connect(ui->tree_project, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onProjectTreeSelect(QTreeWidgetItem*,int)));

    connect(ui->push_add, SIGNAL(pressed()), this, SLOT(onProjectTreeAdd()));
    connect(ui->push_remove, SIGNAL(pressed()), this, SLOT(onProjectTreeRemove()));

    connect(ui->action_new_project, SIGNAL(triggered()), this, SLOT(onNewProject()));
    connect(ui->action_open_project, SIGNAL(triggered()), this, SLOT(onOpenProject()));
    connect(ui->action_save_project, SIGNAL(triggered()), this, SLOT(onSaveProject()));

    connect(ui->action_settings, SIGNAL(triggered()), settings_window, SLOT(show()));

    qsrand(QTime::currentTime().elapsed());

    QFile config_file(QDir::current().absolutePath() + "/config.json");
    if(config_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(config_file.readAll());
        settings_window->set_robogen_directory(doc.object()["build-path"].toString());
    }


    connect(ui->push_evolve, SIGNAL(clicked(bool)), this, SLOT(onEvolve()));
    connect(ui->push_stop, SIGNAL(clicked(bool)), this, SLOT(onPushStop()));
    ui->push_stop->setEnabled(false);

    ui->spin_threads->setMaximum(QThread::idealThreadCount()-1);


    // ========================== OVERVIEW TAB SETUP ===================================
    connect(ui->tree_runs, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onRunSelect(QListWidgetItem*)));
    connect(ui->tree_runs, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onRunSelect(QListWidgetItem*)));

    connect(ui->push_simulate, SIGNAL(clicked(bool)), this, SLOT(onPushSimulate()));
    connect(ui->push_analyze, SIGNAL(clicked(bool)), this, SLOT(onPushAnalyze()));

    connect(ui->tree_runs, SIGNAL(deletePressed()), this, SLOT(onRunDeletePressed()));
    connect(ui->tree_runs, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onRunItemDoubleClicked(QListWidgetItem*)));
    connect(ui->tree_project, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onProjectConfigDoubleClicked(QListWidgetItem*)));

    connect(ui->push_webgl, SIGNAL(clicked(bool)), this, SLOT(onPushWebGL()));

   // connect(ui->tabWidge)

    // ========================== EVOLUTION TAB SETUP =================================

    connect(ui->push_custom_robot_browse, SIGNAL(clicked(bool)), this, SLOT(onCustomRobotBrowse()));
    connect(ui->push_custom_simulation_browse, SIGNAL(clicked(bool)), this, SLOT(onCustomSimulationBrowse()));
    connect(ui->push_custom_evolution_browse, SIGNAL(clicked(bool)), this, SLOT(onCustomEvolutionBrowse()));
    connect(ui->push_obstacles_browse, SIGNAL(clicked(bool)), this, SLOT(onObstaclesBrowse()));
    connect(ui->push_lights_browse, SIGNAL(clicked(bool)), this, SLOT(onLightsBrowse()));
    connect(ui->push_positions_browse, SIGNAL(clicked(bool)), this, SLOT(onPositionsBrowse()));

    //  =========================== ROBOT TAB SETUP ===================================
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

    log_window = new LogWindow(this);

    connect(ui->actionLog, SIGNAL(triggered(bool)), log_window, SLOT(show()));


    // === NEW PROJECT ===
    onNewProject();
    //custom_browser = new CustomBrowser();


}

/*
void MainWindow::loadEvolution()
{

    QString filename = QFileDialog::getOpenFileName(this, "", evolution_filename);
    if(filename != "")
    {
        evolution_filename = filename;
    }

}
*/

/*
void MainWindow::loadSimulation()
{

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

}*/

void MainWindow::onPushSimulate()
{
    QString program = settings_window->get_robogen_directory() + "/robogen-file-viewer";
    QStringList arguments;

    if(!ui->tree_runs->currentItem()) return;
    if(!ui->list_generations->currentItem()) return;
    RunTreeItem* run_item = dynamic_cast<RunTreeItem*>(ui->tree_runs->currentItem());
    QString generation_path = current_config->root_directory + "/" + run_item->path + "/"+ ui->list_generations->currentItem()->text();

    arguments << generation_path << current_config->root_directory + "/" + run_item->path + "/sim.txt";
    process_simulate = new QProcess(this);
    process_simulate->setWorkingDirectory(settings_window->get_robogen_directory());
    process_simulate->start(program, arguments);
}

void MainWindow::onPushWebGL()
{
    QString program = settings_window->get_robogen_directory() + "/robogen-file-viewer";
    QStringList arguments;

    if(!ui->tree_runs->currentItem()) return;
    if(!ui->list_generations->currentItem()) return;
    RunTreeItem* run_item = dynamic_cast<RunTreeItem*>(ui->tree_runs->currentItem());
    QString generation_path = current_config->root_directory + "/" + run_item->path + "/"+ ui->list_generations->currentItem()->text();

    arguments << generation_path
              << current_config->root_directory + "/" + run_item->path + "/sim.txt"<< "1"<<"--output" << project_directory + "/web/"
              << "--no-visualization"
              << "--webgl"
              << "--overwrite";
    process_simulate = new QProcess(this);
    process_simulate->setProcessChannelMode(QProcess::ForwardedChannels);
    process_simulate->setWorkingDirectory(settings_window->get_robogen_directory());
    process_simulate->start(program, arguments);
    ui->push_webgl->setText("SIMULATING");
    process_simulate->waitForFinished();
    ui->push_webgl->setText("WebGL");
    //custom_browser->show();
}

void MainWindow::onPushAnalyze(bool user_activated)
{
    QString path = "";
    if(user_activated) {
        if(!current_config) return;
        if(!ui->tree_runs->currentItem()) return;
        RunTreeItem* item = dynamic_cast<RunTreeItem*>(ui->tree_runs->currentItem());
        path = current_config->root_directory + "/" + item->path + "/BestAvgStd.txt";

    } else {
        path = current_run_path + "/BestAvgStd.txt";
    }

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);

    QVector<double> v_best, v_avg, v_std_up, v_std_down;
    QVector<double> v_x;

    QString line;

    try {

    while(!in.atEnd())
    {
        int x;
        double best, avg, std;
        in.readLineInto(&line);
        sscanf(line.toStdString().c_str(), "%d %lf %lf %lf", &x, &best, &avg, &std);
        v_best.push_back(best);
        v_avg.push_back(avg);
        v_std_up.push_back(avg + std);
        v_std_down.push_back(avg - std);
        v_x.push_back(x);
    }
    }
    catch(...) {
        QMessageBox::critical(this, "Error in analyzer", "Failed to parse BestAvgStd.txt, check the content of file in the folder of the current run.");
        return;
    }

    QCustomPlot *customPlot;
    QCPGraph *graph1;
    QCPGraph *graph2;
    QCPGraph *graph3;
    QCPGraph *graph4;
    if(plot) {
        customPlot = plot;
        graph1 = graph_best;
        graph2 = graph_avg;
        graph3 = graph_std_up;
        graph4 = graph_std_down;
    }else
    {
        customPlot = new QCustomPlot;
        customPlot->legend->setVisible(true);
        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
        customPlot->legend->setTextColor(Qt::white);
        customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 20)));

        graph1 = customPlot->addGraph();
        graph1->setPen(QPen(QColor(255, 255, 255), 2));
        graph1->setName("Average");

        graph2 = customPlot->addGraph();
        graph2->setPen(QPen(QColor(0,0,255,64),2));
        graph2->setBrush(QColor(200, 200, 200, 20));
        graph2->setChannelFillGraph(graph1);
        graph2->setName("STD");

        graph3 = customPlot->addGraph();
        graph3->setPen(QPen(QColor(0,0,255,64),2));
        graph3->setBrush(QColor(200, 200, 200, 20));
        graph3->setChannelFillGraph(graph1);
        graph3->setName("STD");

        graph4 = customPlot->addGraph();
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

        plot = customPlot;
        graph_best = graph1;
        graph_avg = graph2;
        graph_std_up = graph3;
        graph_std_down = graph4;

        customPlot->resize(800,600);
        customPlot->setWindowTitle("Robogen Generations Plot");

    }


    graph1->setData(v_x, v_avg);
    graph2->setData(v_x, v_std_up);
    graph3->setData(v_x, v_std_down);
    graph4->setData(v_x, v_best);

    customPlot->rescaleAxes();
    customPlot->replot();

    if(user_activated) {
        customPlot->show();
    }
}

void MainWindow::onPushStop()
{
    qDebug() << "Killing evolver ..." << endl;
    process_evolve->kill();

    qDebug() << "Killing all servers ..." << endl;
    for(QProcess* p: process_list)
    {
        p->kill();
    }

    process_list.clear();
}

void MainWindow::onFileChanged(QString filename)
{
    if(!run_dir_added) {
        if(QDir(current_run_path).exists()) {
            fs_watcher->addPath(current_run_path);
            run_dir_added = true;
        }
    } else {
        QDir dir(current_run_path);
        QStringList list = dir.entryList({"GenerationBest-*.json"});
        if(list.size() > 0) {
            //qDebug() << "generation " << list.size() << " done" << endl;
            onPushAnalyze(false);
            ui->progressBar->setValue(list.size());
        }

        if(current_config == current_running_config){
            // If the current run is selected in the run list, update the generations list
            if(ui->tree_runs->currentItem() && ui->tree_runs->currentItem()->text() == current_run_name) {
                for(auto& gen_item: list) {
                    // If the item is not yet in the generation list, add it
                    if(ui->list_generations->findItems(gen_item, Qt::MatchExactly).empty()) {
                        ui->list_generations->addItem(gen_item);
                    }
                }
            }
        }
    }

    /*
    ui->list_generationsbest->addItem(filename);
    if(filename.startsWith("Generation"))
    {
        ui->list_generationsbest->addItem(filename);
    }*/

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

void MainWindow::onEvolveFinished(int s)
{
    qDebug() << "evolve finished: " << s << endl;
    delete fs_watcher; fs_watcher = nullptr;

    onPushStop();
    current_running_config = nullptr;
    current_run_path = "";
    ui->push_stop->setEnabled(false);
    ui->push_evolve->setEnabled(true);
}

void MainWindow::onFileEvent(const QString& str)
{
    //qDebug() << str << endl;
}

void MainWindow::onNewProject()
{
    ui->tree_project->clear();
    ui->tabWidget->setEnabled(false);
    current_config = nullptr;
    project_directory = "";
   // onProjectTreeAdd();
   // current_config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(0));
   // ui->tree_project->setCurrentItem(ui->tree_project->topLevelItem(0));
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
        project_directory = "";
        return;
    }

    ui->tree_project->clear();

    QJsonDocument doc = QJsonDocument::fromJson(project_file.readAll());
    QJsonObject main_object = doc.object();
    settings_window->set_robogen_directory(main_object["robogen_directory"].toString());

    ui->spin_threads->setValue(main_object["n_threads"].toInt());

    QJsonArray configs = main_object["configurations"].toArray();
    for(int i = 0; i < configs.size(); i++) {
        QJsonObject config = configs[i].toObject();
        ProjectConfiguration* project1_config = new ProjectConfiguration();
        project1_config->setText(config["name"].toString());
        project1_config->root_directory = config["root_directory"].toString();
        project1_config->setToolTip(project1_config->root_directory);
        qDebug() << "found root_directory: " << project1_config->root_directory;
        ui->tree_project->addItem(project1_config);

        QJsonObject simulation_obj = config["simulation"].toObject();
        QJsonObject evolution_obj = config["evolution"].toObject();
        QJsonObject robot_obj = config["robot"].toObject();
        QJsonArray run_array = config["run_list"].toArray();

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
            else if(key.startsWith("check")) {
                project1_config->simulation->bool_map[key] = simulation_obj.value(key).toBool();
            }
            else if(key.startsWith("line")) {
                project1_config->simulation->text_map[key] = simulation_obj.value(key).toString();
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
            else if(key.startsWith("check")) {
                project1_config->evolution->bool_map[key] = evolution_obj.value(key).toBool();
            }
            else if(key.startsWith("line")) {
                project1_config->evolution->text_map[key] = evolution_obj.value(key).toString();
            }
        }

        for(auto const & run_item: run_array) {
            QJsonObject run_obj = run_item.toObject();
            RunTreeItem* new_run_item = new RunTreeItem();
            new_run_item->setText(run_obj["name"].toString());
            new_run_item->path = run_obj["path"].toString();
            new_run_item->setToolTip(new_run_item->path);
            project1_config->run_list.append(new_run_item);
        }

        for(const QString& key: robot_obj.keys()) {
            /*if(key.startsWith("combo")) {
                project1_config->evolution->combo_map[key] = evolution_obj.value(key).toString();
            }
            else if(key.startsWith("spin")) {
                project1_config->evolution->spin_map[key] = evolution_obj.value(key).toInt();
            }
            else if(key.startsWith("dspin")) {
                project1_config->evolution->double_spin_map[key] = evolution_obj.value(key).toDouble();
            }
            else */if(key.startsWith("check")) {
                project1_config->robot->bool_map[key] = robot_obj.value(key).toBool();
            }
            else if(key.startsWith("line")) {
                project1_config->robot->text_map[key] = robot_obj.value(key).toString();
            }
        }


        QJsonObject robot_definition_obj = robot_obj["definition"].toObject();
        project1_config->robot->loadRobotFromObject(robot_definition_obj);
    }

    //current_config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(0));

    //overview_form->project_path = project_directory;

    //ui->tree_project->setCurrentItem(ui->tree_project->topLevelItem(0));

    ui->tabWidget->setEnabled(false);
}

bool MainWindow::onSaveProject()
{
    if(project_directory == "") {
        project_directory = QFileDialog::getExistingDirectory(this, "Choose project directory", "");
        if(project_directory == "")
            return false;
    }

    QFile project_file(project_directory + "/project.json");
    if(!project_file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        QMessageBox::critical(this, "Saving Error", "Could not open " + project_directory + " for writing\nProject save failed");
        return false;
    }

    saveEvolution();
    saveSimulation();
    saveRobot();

    QJsonDocument doc;
    QJsonObject main_object;

    main_object["robogen_directory"] = settings_window->get_robogen_directory();
    main_object["n_threads"] = ui->spin_threads->value();

    QJsonArray config_array;
    for(int i = 0; i < ui->tree_project->count(); i++) {
        ProjectConfiguration* config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->item(i));
        QJsonObject config_obj;
        QJsonObject evolution_obj;
        QJsonArray run_array;

        for(auto& pair: config->evolution->combo_map) {
            evolution_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->evolution->spin_map) {
            evolution_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->evolution->double_spin_map) {
            evolution_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->evolution->bool_map) {
            evolution_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->evolution->text_map) {
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

        for(auto& pair: config->simulation->bool_map) {
            simulation_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->simulation->text_map) {
            simulation_obj[pair.first] = pair.second;
        }

        for(auto& run_item: config->run_list) {
            QJsonObject run_obj;
            run_obj["name"] = run_item->text();
            run_obj["path"] = run_item->path;
            run_array.push_back(run_obj);
        }

        QJsonObject robot_obj;
        for(auto& pair: config->robot->bool_map) {
            robot_obj[pair.first] = pair.second;
        }

        for(auto& pair: config->robot->text_map) {
            robot_obj[pair.first] = pair.second;
        }

        robot_obj["definition"] = config->robot->get_json();

        config_obj["simulation"] = simulation_obj;
        config_obj["name"] = config->text();
        config_obj["robot"] = robot_obj;
        config_obj["root_directory"] = config->root_directory;
        config_obj["run_list"] = run_array;
        config_array.push_back(config_obj);
    }

    main_object["configurations"] = config_array;

    doc.setObject(main_object);

    project_file.write(doc.toJson());

    //qDebug() << "project_directory: " << project_directory;
    //overview_form->project_path = project_directory;

    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getRobogenPath()
{
    return settings_window->get_robogen_directory();
}


void MainWindow::onEvolve()
{
    if(project_directory == "") {
        QMessageBox::critical(this, "Error", "You need to save the project first");
        return;
    }
    if(!QDir(project_directory + "/tmp").exists()) {
        QDir().mkdir(project_directory + "/tmp");
    }

    if(settings_window->get_robogen_directory() == "") {
        QMessageBox::critical(this, "Error", "Please set the path to the robogen \"build\" or \"run\" folder in File->settings");
        return;
    }

    if(!current_config) {
        QMessageBox::critical(this, "Error", "Please select a configuration from the list that you want to evolve.");
        return;
    }

    writeEvolution();
    writeSimulation();
    writeRobot();

    current_running_config = current_config;
    if(current_running_config->root_directory == "") {
        current_running_config->root_directory = project_directory + "/runs/config_" + QDateTime::currentDateTime().toString("yy_MM_dd__hh_mm_ss__") +  QString::number(ui->tree_runs->count());
        current_running_config->setToolTip(current_running_config->root_directory);
    }

    // Create config output directory if it does not exist yet
    if(!QDir(current_running_config->root_directory).exists()) {
        QDir().mkpath(current_running_config->root_directory);
        qDebug() << "Creating " << current_running_config->root_directory;
    }

    // Create run output directory
    current_run_name = "run_" + QDateTime::currentDateTime().toString("yy_MM_dd__hh_mm_ss__") +  QString::number(ui->tree_runs->count());
    current_run_path = current_running_config->root_directory + "/" + current_run_name;
    qDebug() << "current run path: " << current_run_path;

    RunTreeItem* run_item = new RunTreeItem();
    run_item->setText(current_run_name);
    run_item->path = current_run_name;
    run_item->setToolTip(run_item->path);
    current_running_config->run_list.append(run_item);
    if(current_config == current_running_config) {
        ui->tree_runs->addItem(run_item);
    }

    QString robogen_path = settings_window->get_robogen_directory();
#if defined(Q_OS_WIN)
    QFile robogen_server_file(robogen_path + "/robogen-server.exe");
#elif defined(Q_OS_LINUX)
    QFile robogen_server_file(robogen_path + "/robogen-server");
#endif
    if(!robogen_server_file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not find robogen-server in the robogen directory, please check file->settings");
        return;
    }

    log_window->onNewEvolution();

    ui->push_evolve->setDisabled(true);
    ui->push_stop->setEnabled(true);
    ui->progressBar->setValue(0);

    // Launch the server(s)
    {
        QString program = robogen_path + "/robogen-server";
        QStringList arguments;
        for(int i = 0; i < ui->spin_threads->value(); i++)
        {
            arguments.clear();
            arguments << QString::number(8000 + i + 1);
            QProcess *process = new QProcess(this);
            //process->setProcessChannelMode(QProcess::ForwardedErrorChannel);
            process->start(program, arguments);
            process_list.push_back(process);
            connect(process, SIGNAL(readyReadStandardOutput()), log_window, SLOT(onServerLog()));
            connect(process, SIGNAL(readyReadStandardError()), log_window, SLOT(onServerError()));

            //connect(process, SIGNAL()
            //qDebug() << "Launching server #" << i + 1 << endl;;
        }

        //qDebug() << "Launching server #" << ui->spin_threads->value() << endl;
    }

    // Create file system watcher so we can check for new generations
    {

        if(fs_watcher) delete fs_watcher;
        fs_watcher = new QFileSystemWatcher(this);
        fs_watcher->addPath(current_running_config->root_directory);
        run_dir_added = false;
        connect(fs_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onFileChanged(QString)));
        qDebug() << "watching " << fs_watcher->directories() << endl;
    }

    ui->progressBar->setMaximum(ui->spin_generations->value());

    // Launch an evolution
    {
        QString program = robogen_path + "/robogen-evolver";
        QStringList arguments;
        QString seed;
        if(ui->check_randomseed->isChecked())
            seed = QString::number(qrand());
        else
            seed = QString::number(ui->spin_seed->value());

        if(ui->check_custom_robot->isChecked()) {
            QFile(project_directory + "/tmp/robot.txt").remove();
            // if the robot is in the json format, we need to convert it to the txt format using
            // the python tool because we are lazy
            if(ui->line_custom_robot->text().split(".").back() == "json") {
                QProcess process_convert;
                process_convert.setProcessChannelMode(QProcess::ForwardedChannels);
                QStringList conv_args;
                conv_args << robogen_path + "/../python/json_converter.py" << ui->line_custom_robot->text() << project_directory + "/tmp/robot.txt";
                process_convert.start("python", conv_args);
                if(!process_convert.waitForFinished(1000)) {
                    QMessageBox::critical(this, "Error", "Robot format conversion failed!!!");
                }
            } else {
                QFile::copy(ui->line_custom_robot->text(), project_directory + "/tmp/robot.txt");
            }
        }

        if(ui->check_custom_simulation->isChecked()) {
            QFile(project_directory + "/tmp/sim.txt").remove();
            QFile::copy(ui->line_custom_simulation->text(), project_directory + "/tmp/sim.txt");
        }

        if(ui->check_custom_evolution->isChecked()) {
            QFile(project_directory + "/tmp/evo.txt").remove();
            QFile::copy(ui->line_custom_evolution->text(), project_directory + "/tmp/evo.txt");
        }
        arguments << seed << current_run_path << project_directory + "/tmp/evo.txt" << "--overwrite";

        qDebug() << "arguments: " << arguments << endl;

        process_evolve = new QProcess(this);
        connect(process_evolve, SIGNAL(readyReadStandardOutput()), log_window, SLOT(onEvolverLog()));
        connect(process_evolve, SIGNAL(readyReadStandardError()), log_window, SLOT(onEvolverError()));

        //process_evolve->setProcessChannelMode(QProcess::ForwardedErrorChannel);
        process_evolve->start(program, arguments);


        connect(process_evolve, SIGNAL(finished(int)), this, SLOT(onEvolveFinished(int)));
        process_evolve->waitForStarted();
    }

    // dir_watcher.watch(project_directory + "/tmp/run/",100);
}

void MainWindow::saveSimulation()
{
    if(current_config) {
        //qDebug() << "saveSimulation" << endl;
        for(QComboBox *child: ui->tab_simulation->findChildren<QComboBox*>()) {
            current_config->simulation->combo_map[child->objectName()] = child->currentText();
        }

        for(QSpinBox* child: ui->tab_simulation->findChildren<QSpinBox*>()) {
            current_config->simulation->spin_map[child->objectName()] = child->value();
        }

        for(QDoubleSpinBox* child: ui->tab_simulation->findChildren<QDoubleSpinBox*>()) {
            current_config->simulation->double_spin_map[child->objectName()] = child->value();
        }

        for(QCheckBox* child: ui->tab_simulation->findChildren<QCheckBox*>()) {
            current_config->simulation->bool_map[child->objectName()] = child->isChecked();
        }

        for(QLineEdit* child: ui->tab_simulation->findChildren<QLineEdit*>()) {
            current_config->simulation->text_map[child->objectName()] = child->text();
        }
    }
}

void MainWindow::loadSimulation()
{
    if(current_config) {
        //qDebug() << "loadSimulation" << endl;
        for(auto& pair: current_config->simulation->combo_map) {
            QComboBox* child = ui->tab_simulation->findChild<QComboBox*>(pair.first);
            child->setCurrentText(pair.second);
        }

        for(auto& pair: current_config->simulation->spin_map) {
            QSpinBox* child = ui->tab_simulation->findChild<QSpinBox*>(pair.first);
            child->setValue(pair.second);
        }

        for(auto& pair: current_config->simulation->double_spin_map) {
            QDoubleSpinBox* child = ui->tab_simulation->findChild<QDoubleSpinBox*>(pair.first);
            child->setValue(pair.second);
        }

        for(auto& pair: current_config->simulation->bool_map) {
            QCheckBox* child = ui->tab_simulation->findChild<QCheckBox*>(pair.first);
            child->setChecked(pair.second);
        }

        for(auto& pair: current_config->simulation->text_map) {
            QLineEdit* child = ui->tab_simulation->findChild<QLineEdit*>(pair.first);
            child->setText(pair.second);
        }
    }
}

void MainWindow::saveEvolution()
{
    if(current_config) {
        //qDebug() << "saveEvolution" << endl;
        for(QComboBox *child: ui->tab_evolution->findChildren<QComboBox*>()) {
            current_config->evolution->combo_map[child->objectName()] = child->currentText();
        }

        for(QSpinBox* child: ui->tab_evolution->findChildren<QSpinBox*>()) {
            current_config->evolution->spin_map[child->objectName()] = child->value();
        }

        for(QDoubleSpinBox* child: ui->tab_evolution->findChildren<QDoubleSpinBox*>()) {
            current_config->evolution->double_spin_map[child->objectName()] = child->value();
        }

        for(QCheckBox* child: ui->tab_evolution->findChildren<QCheckBox*>()) {
            current_config->evolution->bool_map[child->objectName()] = child->isChecked();
        }

        for(QLineEdit* child: ui->tab_evolution->findChildren<QLineEdit*>()) {
            current_config->evolution->text_map[child->objectName()] = child->text();
        }
    }
}

void MainWindow::loadEvolution()
{
    if(current_config) {
        //qDebug() << "loadEvolution" << endl;
        for(auto& pair: current_config->evolution->combo_map) {
            QComboBox* child = ui->tab_evolution->findChild<QComboBox*>(pair.first);
            child->setCurrentText(pair.second);
        }

        for(auto& pair: current_config->evolution->spin_map) {
            QSpinBox* child = ui->tab_evolution->findChild<QSpinBox*>(pair.first);
            child->setValue(pair.second);
        }

        for(auto& pair: current_config->evolution->double_spin_map) {
            QDoubleSpinBox* child = ui->tab_evolution->findChild<QDoubleSpinBox*>(pair.first);
            child->setValue(pair.second);
        }

        for(auto& pair: current_config->evolution->bool_map) {
            QCheckBox* child = ui->tab_evolution->findChild<QCheckBox*>(pair.first);
            child->setChecked(pair.second);
        }

        for(auto& pair: current_config->evolution->text_map) {
            QLineEdit* child = ui->tab_evolution->findChild<QLineEdit*>(pair.first);
            child->setText(pair.second);
        }
    }
}

void MainWindow::saveRobot()
{
    if(current_config) {
        //qDebug() << "saveRobot" << endl;
        //ui->robotConfigTree->takeTopLevelItem(0);

        for(QCheckBox* child: ui->tab_robot->findChildren<QCheckBox*>()) {
            current_config->robot->bool_map[child->objectName()] = child->isChecked();
        }

        for(QLineEdit* child: ui->tab_robot->findChildren<QLineEdit*>()) {
            current_config->robot->text_map[child->objectName()] = child->text();
        }
    }
}

void MainWindow::loadRobot()
{
    if(current_config) {
        //qDebug() << "loadRobot" << endl;
        ui->robotConfigTree->addTopLevelItem(current_config->robot->root_part);
        ui->robotConfigTree->expandAll();

        for(auto& pair: current_config->robot->bool_map) {
            QCheckBox* child = ui->tab_robot->findChild<QCheckBox*>(pair.first);
            child->setChecked(pair.second);
        }

        for(auto& pair: current_config->robot->text_map) {
            QLineEdit* child = ui->tab_robot->findChild<QLineEdit*>(pair.first);
            child->setText(pair.second);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    // if this project is already saved
    if(project_directory != "")
        onSaveProject();
    ev->accept();
}

void MainWindow::onProjectTreeSelect(QListWidgetItem *item_)
{
    ui->robotConfigTree->takeTopLevelItem(0);
    if(current_config) {
        saveSimulation();
        saveEvolution();
        saveRobot();
    }

    while(ui->tree_runs->count() > 0) {
        ui->tree_runs->takeItem(0);
    }

    ui->list_generations->clear();

    if(item_ == nullptr) {
        ui->tabWidget->setEnabled(false);
    } else {
        ui->tabWidget->setEnabled(true);
    }

    current_config = dynamic_cast<ProjectConfiguration*>(item_);
    if(current_config) {
        loadSimulation();
        loadEvolution();
        loadRobot();
        for(auto& run_item: current_config->run_list) {
            ui->tree_runs->addItem(run_item);
        }
    }
}

void MainWindow::onProjectTreeAdd()
{
    ProjectConfiguration* project1_config = new ProjectConfiguration();

    project1_config->setText("New Configuration");
    ui->tree_project->addItem(project1_config);
}

void MainWindow::onProjectTreeRemove()
{
    ProjectConfiguration* item = dynamic_cast<ProjectConfiguration*>(ui->tree_project->currentItem());
    if(item == current_running_config) {
        QMessageBox::critical(this, "Error", "You cannot delete the currently running configuration, stop it first");
        return;
    }
    if(item != nullptr) {
        if(item->root_directory != "") QDir(item->root_directory).removeRecursively();
        delete item;
    }
    ui->tree_project->setCurrentItem(nullptr);
    ui->tabWidget->setEnabled(false);

    current_config = nullptr;
}


void MainWindow::onItemChange(QTreeWidgetItem*, QTreeWidgetItem*)
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

void MainWindow::onRunSelect(QListWidgetItem* current)
{
    ui->list_generations->clear();
    if(!current) return;
    RunTreeItem* item = dynamic_cast<RunTreeItem*>(current);

    QString run_directory = current_config->root_directory + "/" + item->path;
    //qDebug() << "checking run directory: " << run_directory << endl;
    QDir dir(run_directory);
    QStringList list = dir.entryList({"GenerationBest-*.json"});
    ui->list_generations->addItems(list);
}

void MainWindow::onRunDeletePressed()
{
    if(!ui->tree_runs->currentItem()) return;
    RunTreeItem* item = dynamic_cast<RunTreeItem*>(ui->tree_runs->currentItem());
    if(current_run_path.split("/").last() == item->path) {
        QMessageBox::warning(this, "Warning", "You are trying to delete the currently running evolution, please stop it first.");
        return;
    }
    QDir(current_config->root_directory + "/" + item->path).removeRecursively();
    current_config->run_list.removeOne(item);
    delete ui->tree_runs->currentItem();
    ui->list_generations->clear();
}

void MainWindow::onRunItemDoubleClicked(QListWidgetItem *item)
{
    item->setFlags(item->flags() |= Qt::ItemIsEditable);
}

void MainWindow::onProjectConfigDoubleClicked(QListWidgetItem *item)
{
    item->setFlags(item->flags() |= Qt::ItemIsEditable);
}

void MainWindow::onLoadRobot()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose robot config file", "", "*.json *.txt");
    if(filename == "")
        return;

    current_config->robot->loadRobot(filename);
    loadRobot();

}

void MainWindow::loadRobotJson(const QString &filename)
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
    current_config->robot->root_part = new_root_part;
    current_config->robot->root_part->update();
    loadRobot();
}

void MainWindow::onCustomRobotBrowse()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose robot config file", "", "*.json *.txt");
    if(filename == "")
        return;

    ui->line_custom_robot->setText(filename);
}

void MainWindow::onCustomSimulationBrowse()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose scenario config file", "", "*.txt");
    if(filename == "")
        return;

    ui->line_custom_scenario->setText(filename);
}

void MainWindow::onCustomEvolutionBrowse()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose evolution config file", "", "*.txt");
    if(filename == "")
        return;

    ui->line_custom_evolution->setText(filename);
}

void MainWindow::onObstaclesBrowse()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose obstacle config file", "", "*.txt");
    if(filename == "")
        return;

    ui->line_obstacles->setText(filename);
}

void MainWindow::onPositionsBrowse()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose position config file", "", "*.txt");
    if(filename == "")
        return;

    ui->line_positions->setText(filename);
}

void MainWindow::onLightsBrowse()
{
    if(!current_config)
        return;

    QString filename = QFileDialog::getOpenFileName(this,"Choose light config file", "", "*.txt");
    if(filename == "")
        return;

    ui->line_lights->setText(filename);
}


void MainWindow::writeEvolution()
{
    QString filename = project_directory + "/tmp/evo.txt";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream << "# GENERATED BY ROBOGEN GUI" << endl;

        stream <<"simulatorConfFile=" << project_directory + "/tmp/sim.txt" << endl;
        stream <<"referenceRobotFile=" << project_directory + "/tmp/robot.txt" << endl;

        stream <<"mu=" << ui->spin_mu->value() << endl;
        stream <<"lambda=" << ui->spin_lambda->value() << endl;
        stream <<"selection="<< ui->combo_selection->currentText() << endl;
        stream <<"replacement="<< ui->combo_replacement->currentText()<< endl;
        stream <<"tournamentSize="<< ui->spin_tournament->value() << endl;
        stream <<"numGenerations="<< ui->spin_generations->value() << endl;
        stream <<"evolutionMode="<< ui->combo_evolutionmode->currentText() << endl;
        stream <<"pBrainMutate="<< ui->dspin_pbrainmutate->value() << endl;
        stream <<"brainSigma="<< ui->dspin_brainsigma->value() << endl;
        stream <<"pBrainCrossover="<< ui->dspin_pbraincrossover->value() << endl;
        stream <<"addBodyPart="<< "All" << endl;                                        ///TODO: Add to config
        stream <<"pNodeInsert=" << ui->dspin_pnodeinsert->value() << endl;
        stream <<"pSubtreeRemove=" << ui->dspin_psubtreeremove->value() << endl;
        stream <<"pSubtreeDuplicate=" << ui->dspin_psubtreeduplicate->value() << endl;
        stream <<"pSubtreeSwap=" << ui->dspin_psubtreeswap->value() << endl;
        stream <<"pNodeRemove=" << ui->dspin_pnoderemove->value() << endl;
        stream <<"pParameterModify=" << ui->dspin_pparemetermodify->value() << endl;
        stream <<"brainBounds=-3:3" << endl;                                            ///TODO: Add to configurations
        stream <<"pAddHiddenNeuron=" << ui->dspin_paddhiddenneuron->value() << endl;
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
}

void MainWindow::writeSimulation()
{
    QString filename = project_directory + "/tmp/sim.txt";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream << "# GENERATED BY ROBOGEN GUI" << endl;
        if(ui->check_custom_scenario->isChecked())
            stream <<"scenario=" << ui->line_custom_scenario->text() << endl;
        else
            stream <<"scenario=" << ui->combo_scenario->currentText() << endl;
        stream <<"timeStep=" << ui->dspin_timestep->value() << endl;
        stream <<"actuationFrequency="<< ui->dspin_actuationfrequency->value() << endl;
        stream <<"nTimeSteps="<< ui->spin_ntimesteps->value() << endl;
        stream <<"terrainType="<< ui->combo_terraintype->currentText() << endl;
        stream <<"terrainLength="<< ui->dspin_terrainlength->value() << endl;
        stream <<"terrainWidth="<< ui->dspin_terrainwidth->value() << endl;
        stream <<"terrainFriction="<< ui->dspin_terrainfriction->value() << endl;
        stream <<"sensorNoiseLevel="<< ui->dspin_sensornoiselevel->value() << endl;
        stream <<"motorNoiseLevel="<< ui->dspin_motornoiselevel->value() << endl;
        stream <<"capAcceleration="<< (ui->check_capacceleration->isChecked() ? "true": "false") << endl;
        if(ui->line_obstacles->text().length() > 0) {
            stream <<"obstaclesConfigFile="<< ui->line_obstacles->text() << endl;
        }
        if(ui->line_positions->text().length() > 0) {
            stream <<"startPositionConfigFile=" << ui->line_positions->text() << endl;
        }

        if(ui->line_lights->text().length() > 0) {
            stream <<"lightSourcesConfigFile=" << ui->line_lights->text() << endl;
        }

    }
    else
    {
        qDebug() << "FAILED TO SAVE SIMULATION TO: " << filename << endl;
        QMessageBox::critical(this, "Error", "Failed to save simulation config to: " + filename);
    }
}
/*
QJsonObject MainWindow::getPartJsonObject(RobotPart* part)
{
    QJsonObject obj;
    QJsonObject obj;
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

    return obj;
    return obj;
}*/

QJsonObject MainWindow::getRobotJsonObject()
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

void MainWindow::onAddPart()
{
    if(!current_config)
        return;
    RobotPart *part = new RobotPart;
    current_config->robot->root_part->addChild(part);
    ui->robotConfigTree->clearSelection();
    ui->robotConfigTree->setCurrentItem(part);
}

void MainWindow::onRemovePart()
{
    QTreeWidgetItem* item = ui->robotConfigTree->currentItem();
    //printf("item: %p\n", item);
    if(!item)
        return;

    if(item == current_config->robot->root_part)
        return;

    if(!item->parent())
        return;

    QModelIndex index =  ui->robotConfigTree->currentIndex();
    QModelIndex index2 = index.sibling(index.row() - 1, 0);
    ui->robotConfigTree->setCurrentIndex(index2);
    item->parent()->removeChild(item);
}


void MainWindow::onRobotPartChange(int index)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->type = (PART_TYPE)index;
    part->update();
}

void MainWindow::onRobotFaceChange(int index)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    part->face = (PART_FACE)index;
    part->update();
}

void MainWindow::onRobotPartNameChange(QString name)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    part->name = name;
    part->update();
}

void MainWindow::onRobotRotationChange(int index)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    part->rotation = index;
    part->update();
}

void MainWindow::onRobotParamLengthChange(double value)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_length = value;
    }
}

void MainWindow::onRobotParamRotationChange(int value)
{
    RobotPart* part = dynamic_cast<RobotPart*>(ui->robotConfigTree->currentItem());
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_rotation = value;
    }
}


PART_TYPE MainWindow::typeFromString(const QString& str)
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


void writeRobotPart(RobotPart* part, int tab, QTextStream& stream)
{
    QString name;
    QString type = MainWindow::stringFromType(part->type);

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
}

void MainWindow::writeRobot()
{
    QString filename = project_directory + "/tmp/robot.txt";

    QFile file(filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        writeRobotPart(current_config->robot->root_part, 0, stream);
        //FIXME Dirty hack
        //stream << "\n\n\nLeftWheel 0 100" << endl;
        //stream << "RightWheel 0 -100" << endl;
    }
    else
    {
        qDebug() << "FAILED TO SAVE ROBOT TO: " << filename << endl;
    }
}

QString MainWindow::stringFromType(PART_TYPE tp)
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

