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

#include <chrono>
#include <thread>

using namespace std::chrono;

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

    overview_form->simulation_form = simulation_config_form;
    overview_form->evolution_form = evolution_config_form;
    overview_form->robot_form = robot_config_form;

    overview_form->main_window = this;

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

    qsrand(QTime::currentTime().elapsed());

    QFile config_file(QDir::current().absolutePath() + "/config.json");
    if(config_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(config_file.readAll());
        settings_window->set_robogen_directory(doc.object()["build-path"].toString());
    }
    onNewProject();

    connect(ui->push_evolve, SIGNAL(clicked(bool)), this, SLOT(onEvolve()));
    connect(ui->push_stop, SIGNAL(clicked(bool)), this, SLOT(onPushStop()));
    ui->push_stop->setEnabled(false);

    ui->spin_threads->setMaximum(QThread::idealThreadCount()-1);


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
            qDebug() << "generation " << list.size() << " done" << endl;
            ui->progressBar->setValue(list.size());
            if(current_config == current_running_config) {

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

    ui->push_stop->setEnabled(false);
    ui->push_evolve->setEnabled(true);
}

void MainWindow::onFileEvent(const QString& str)
{
    qDebug() << str << endl;
}

void MainWindow::onNewProject()
{
    ui->tree_project->clear();
    onProjectTreeAdd();
    ProjectConfiguration* current_config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(0));
    simulation_config_form->current_config = current_config->simulation;
    simulation_config_form->hide();

    evolution_config_form->current_config = current_config->evolution;
    evolution_config_form->hide();

    robot_config_form->current_config = current_config->robot;
    robot_config_form->hide();

    overview_form->show();
    overview_form->current_config = current_config;
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
        project1_config->root_directory = config["root_directory"].toString();
        qDebug() << "found root_directory: " << project1_config->root_directory;
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

        QJsonObject robot_obj = config["robot"].toObject();
        project1_config->robot->loadRobotFromObject(robot_obj);
    }

    current_config = dynamic_cast<ProjectConfiguration*>(ui->tree_project->topLevelItem(0));

    simulation_config_form->current_config = current_config->simulation;
    simulation_config_form->loadAll();
    simulation_config_form->hide();

    evolution_config_form->current_config = current_config->evolution;
    evolution_config_form->loadAll();
    evolution_config_form->hide();

    robot_config_form->current_config = current_config->robot;
    robot_config_form->loadAll();
    robot_config_form->hide();

    overview_form->show();
    overview_form->current_config = current_config;
    overview_form->loadAll();

    //overview_form->project_path = project_directory;

    ui->tree_project->setCurrentItem(ui->tree_project->topLevelItem(0));
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
        config_obj["robot"] = config->robot->get_json();
        config_obj["root_directory"] = config->root_directory;
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
        QMessageBox::critical(this, "Error", "Please set the path to the robogen \"build\" folder in File->settings");
        return;
    }
    current_running_config = current_config;
    if(current_running_config->root_directory == "") {
        current_running_config->root_directory = project_directory + "/runs/config_" + QString::number(time(0));
    }

    // Create config output directory if it does not exist yet
    if(!QDir(current_running_config->root_directory).exists()) {
        QDir().mkpath(current_running_config->root_directory);
        qDebug() << "Creating " << current_running_config->root_directory;
    }

    // Create run output directory
    current_run_path = current_running_config->root_directory + "/run_" + QString::number(time(0));
    qDebug() << "current run path: " << current_run_path;

    overview_form->robot_form->writeRobot(project_directory);
    overview_form->evolution_form->writeEvolution(project_directory, ui->spin_threads->value());
    overview_form->simulation_form->writeSimulation(project_directory);


    ui->push_evolve->setDisabled(true);
    ui->push_stop->setEnabled(true);
    ui->progressBar->setValue(0);

    // Make sure to kill any running servers to avoid conflicts
    ///FIXME: Maybe move the killing into a separate method and keep the callback just for the button press?
    //onPushStop();

    QString robogen_path = settings_window->get_robogen_directory();

    // Launch the server(s)
    {
        QString program = robogen_path + "/robogen-server";
        QStringList arguments;
        for(int i = 0; i < ui->spin_threads->value(); i++)
        {
            arguments.clear();
            arguments << QString::number(8000 + i + 1);
            QProcess *process = new QProcess(this);
            //process->setProcessChannelMode(QProcess::ForwardedChannels);
            process->start(program, arguments);
            process_list.push_back(process);

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

    ui->progressBar->setMaximum(evolution_config_form->getMaxGenerations());

    // Launch an evolution
    {
        QString program = robogen_path + "/robogen-evolver";
        QStringList arguments;
        QString seed;
        /*if(ui->check_randomseed->isChecked())
            seed = QString::number(qrand());
        else
            seed = QString::number(ui->spin_seed->value());*/
        seed = "0";
        arguments << seed << current_run_path << project_directory + "/tmp/evo.txt" << "--overwrite";
        qDebug() << "arguments: " << arguments << endl;

        process_evolve = new QProcess(this);
        process_evolve->setProcessChannelMode(QProcess::ForwardedErrorChannel);
        process_evolve->start(program, arguments);

        connect(process_evolve, SIGNAL(finished(int)), this, SLOT(onEvolveFinished(int)));
        process_evolve->waitForStarted();
    }

    // dir_watcher.watch(project_directory + "/tmp/run/",100);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    // if this project is already saved
    if(project_directory != "")
        onSaveProject();
    ev->accept();
}

void MainWindow::onProjectTreeSelect(QTreeWidgetItem *item_, int)
{
    if(!robot_config_form->isHidden()) robot_config_form->saveAll();
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
        overview_form->current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        break;
    case ItemClass::ROBOT:
        robot_config_form->show();
        robot_config_form->current_config = dynamic_cast<RobotConfiguration*>(item);
        robot_config_form->loadAll();
        overview_form->current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        overview_form->loadAll();
        current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        break;
    case ItemClass::SIMULATION:
        simulation_config_form->show();
        simulation_config_form->current_config = dynamic_cast<SimulationConfiguration*>(item);
        simulation_config_form->loadAll();
        overview_form->current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        overview_form->loadAll();
        current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        break;
    case ItemClass::PROJECT:
        overview_form->show();
        overview_form->current_config = dynamic_cast<ProjectConfiguration*>(item);
        evolution_config_form->current_config = overview_form->current_config->evolution;
        simulation_config_form->current_config = overview_form->current_config->simulation;
        robot_config_form->current_config = overview_form->current_config->robot;
        evolution_config_form->loadAll();
        simulation_config_form->loadAll();
        robot_config_form->loadAll();
        overview_form->loadAll();
        current_config = dynamic_cast<ProjectConfiguration*>(item->parent());
        break;
    default:
        break;
    }
    //overview_form->project_path = project_directory;
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
    // Prevent removing the last configuration from the project
    if(ui->tree_project->topLevelItemCount() < 2)
        return;
    ConfigurationItem* item = dynamic_cast<ConfigurationItem*>(ui->tree_project->currentItem());
    if(item != nullptr && item->getType() == ItemClass::PROJECT) {
        delete item;
    }
}

