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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->button_add_part, SIGNAL(clicked(bool)), this, SLOT(onAddPart()));
    connect(ui->robotConfigTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->robotPartCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onRobotPartChange(int)));
    connect(ui->robotFaceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onRobotFaceChange(int)));
    connect(ui->robotRotateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onRobotRotationChange(int)));
    connect(ui->spin_paramlength, SIGNAL(valueChanged(double)), this, SLOT(onRobotParamLengthChange(double)));
    connect(ui->spin_paramrotation, SIGNAL(valueChanged(int)), this, SLOT(onRobotParamRotationChange(int)));

    connect(ui->robotPartNameEdit, SIGNAL(textEdited(QString)), this, SLOT(onRobotPartNameChange(QString)));

    connect(ui->button_remove_part, SIGNAL(clicked(bool)), this, SLOT(onRemovePart()));

    connect(ui->action_loadsimulation, SIGNAL(triggered(bool)), this, SLOT(loadSimulation()));
    connect(ui->action_loadevolution, SIGNAL(triggered(bool)), this, SLOT(loadEvolution()));
    connect(ui->action_loadrobot, SIGNAL(triggered(bool)), this, SLOT(loadRobot()));

    connect(ui->action_savesimulation, SIGNAL(triggered(bool)), this, SLOT(saveSimulation()));
    connect(ui->action_saveevolution, SIGNAL(triggered(bool)), this, SLOT(saveEvolution()));
    connect(ui->action_saverobot, SIGNAL(triggered(bool)), this, SLOT(saveRobot()));

    connect(ui->push_saveall, SIGNAL(clicked(bool)), this, SLOT(saveAll()));

    connect(ui->push_robogenbrowse, SIGNAL(clicked(bool)), this, SLOT(browseRobogen()));
    connect(ui->push_browseevolve, SIGNAL(clicked(bool)), this, SLOT(browseEvolve()));
    connect(ui->push_browsescenario, SIGNAL(clicked(bool)), this, SLOT(browseCustomScenario()));

    connect(ui->push_evolve, SIGNAL(clicked(bool)), this, SLOT(onPushEvolve()));
    connect(ui->push_simulate, SIGNAL(clicked(bool)), this, SLOT(onPushSimulate()));
    connect(ui->push_analyze, SIGNAL(clicked(bool)), this, SLOT(onPushAnalyze()));

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

    ui->spin_threads->setMinimum(1);
    ui->spin_threads->setMaximum(QThread::idealThreadCount()-1);
    ui->spin_threads->setValue(1);

    qsrand(QTime::currentTime().elapsed());
}

void MainWindow::onAddPart()
{
    RobotPart *part = new RobotPart;
    root_part->addChild(part);
    ui->robotConfigTree->clearSelection();
    ui->robotConfigTree->setCurrentItem(part);
}

void MainWindow::onRemovePart()
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

void MainWindow::onItemChange(QTreeWidgetItem*, QTreeWidgetItem*)
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
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->face = (PART_FACE)index;
    part->update();
}

void MainWindow::onRobotPartNameChange(QString name)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->name = name;
    part->update();
}

void MainWindow::onRobotRotationChange(int index)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    part->rotation = index;
    part->update();
}

void MainWindow::onRobotParamLengthChange(double value)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_length = value;
    }
}

void MainWindow::onRobotParamRotationChange(int value)
{
    RobotPart* part = (RobotPart*)ui->robotConfigTree->currentItem();
    if(!part)
        return;

    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        part->param_rotation = value;
    }
}

void MainWindow::loadSimulation()
{
    QString filename = QFileDialog::getOpenFileName(this, "", simulation_filename);
    if(filename != "")
    {
        simulation_filename = filename;
    }
}

void MainWindow::loadEvolution()
{
    QString filename = QFileDialog::getOpenFileName(this, "", evolution_filename);
    if(filename != "")
    {
        evolution_filename = filename;
    }
}

void MainWindow::loadRobot()
{
    QString filename = QFileDialog::getOpenFileName(this, "", robot_filename);
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
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString line;

    ui->robotConfigTree->clear();
    robot_part_hash.clear();

    root_part = new RobotPart;
    root_part->type = PART_TYPE::CORE_COMPONENT;

    in.readLine();
    RobotPart* current_parent = root_part;
    RobotPart *new_part;
    RobotPart *prev_part;
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

void MainWindow::loadRobotJson(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject object = doc.object();

    QJsonArray parts = object["body"].toObject()["part"].toArray();

    QHash<QString, RobotPart*> hash;

    RobotPart *new_root_part = new RobotPart;
    foreach (const QJsonValue & part, parts)
    {
        RobotPart *new_part = new RobotPart;
        new_part->name = part.toObject()["id"].toString();
        new_part->type = typeFromString(part.toObject()["type"].toString());
        new_part->rotation = part.toObject()["orientation"].toInt();
        if(new_part->type == PART_TYPE::CORE_COMPONENT)
            new_root_part = new_part;
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

    root_part = new_root_part;
    root_part->update();
    ui->robotConfigTree->clear();
    ui->robotConfigTree->addTopLevelItem(root_part);
    ui->robotConfigTree->expandAll();
}

void MainWindow::saveSimulation()
{
    QString filename = simulation_filename;
    if(filename == "")
    {
        filename = QFileDialog::getSaveFileName(this, "", simulation_filename);
    }

    if(filename != "")
    {
        simulation_filename = filename;
        QFile file(simulation_filename);
        if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
        {
            QTextStream stream( &file );
            stream << "# GENERATED BY ROBOGEN GUI by Peter Lichard" << endl;
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
        }
    }
}

void MainWindow::saveEvolution()
{
    QString filename = evolution_filename;
    if(filename == "")
    {
        filename = QFileDialog::getSaveFileName(this, "", evolution_filename);
    }

    if(filename != "")
    {
        evolution_filename = filename;
        QFile file(evolution_filename);
        if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
        {
            QTextStream stream( &file );
            stream << "# GENERATED BY ROBOGEN GUI by Peter Lichard" << endl;
            stream <<"simulatorConfFile=" << simulation_filename << endl;
            stream <<"referenceRobotFile=" << robot_filename << endl;
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
            stream <<"addBodyPart="<< "All" << endl;
            stream <<"pNodeInsert=" << ui->spin_pnodeinsert->value() << endl;
            stream <<"pSubtreeRemove=" << ui->spin_psubtreeremove->value() << endl;
            stream <<"pSubtreeDuplicate=" << ui->spin_psubtreeduplicate->value() << endl;
            stream <<"pSubtreeSwap=" << ui->spin_psubtreeswap->value() << endl;
            stream <<"pNodeRemove=" << ui->spin_pnoderemove->value() << endl;
            stream <<"pParameterModify=" << ui->spin_pparemetermodify->value() << endl;
            stream <<"brainBounds=-3:3" << endl;                                            ///TODO: Add to configurations
            for(int i = 0; i < ui->spin_threads->value(); i++)
            {
                stream <<"socket=127.0.0.1:800"<<i+1<<endl;
            }
        }
    }
}

void writeRobotPart(RobotPart* part, int tab, QTextStream& stream)
{
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
    }

    for(int i = 0; i < tab; i++)
    {
        stream << "\t";
    }
    name = part->name;
    if(name.isEmpty())
        name = "PARTGEN_" + QString::number((long int)part);

    stream << (int)part->face <<" " << type <<" "<< name << " " << (int)part->rotation;
    if(part->type == PART_TYPE::PARAMETRIC_JOINT)
    {
        // The last param is the inclination, which is disabled for now and force to 0
        stream << " " << part->param_length << " " << part->param_rotation <<" 0";
    }
    stream << endl;

    for(int i = 0; i < part->childCount(); i++)
    {
        writeRobotPart(dynamic_cast<RobotPart*>(part->child(i)), tab+1,stream);
    }
}

void MainWindow::saveRobot()
{
    /*QString filename = QFileDialog::getSaveFileName(this, "", robot_filename);
    if(filename != "")
    {
        robot_filename = filename;
    }*/
    QFile file(robot_filename);
    if ( file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        writeRobotPart(root_part, 0, stream);
    }
}

void MainWindow::saveAll()
{
    if(project_path.isEmpty())
    {
        QString path = QFileDialog::getExistingDirectory(this);
        project_path = path;
        simulation_filename = project_path + "/sim.txt";
        evolution_filename = project_path + "/evo.txt";
        robot_filename = project_path + "/robot.txt";
    }

    if(!project_path.isEmpty())
    {
        saveRobot();
        saveEvolution();
        saveSimulation();
    }
}

void MainWindow::browseRobogen()
{
    QString path = QFileDialog::getExistingDirectory(this);

    robogen_path = path;
    ui->edit_robogenpath->setText(robogen_path);
}

void MainWindow::browseEvolve()
{
    QString path = QFileDialog::getExistingDirectory(this);

    evolve_path = path;
    ui->line_evolve->setText(evolve_path);
}

void MainWindow::browseCustomScenario()
{
    QString filename = QFileDialog::getOpenFileName(this);
    ui->line_scenario->setText(filename);
}

void MainWindow::onPushEvolve()
{   
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

    dir.removeRecursively();


    if(ui->edit_robogenpath->text().isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("ERROR");
        msgBox.setText("Path to robogen build folder is not set.");
        msgBox.exec();
        return;
    }

    ui->push_evolve->setDisabled(true);

    // Launch the server(s)
    {
        QString program = ui->edit_robogenpath->text() + "/robogen-server";
        QStringList arguments;
        for(int i = 0; i < ui->spin_threads->value()-1; i++)
        {
            arguments.clear();
            arguments << QString::number(8000 + i + 1);
            process_server = new QProcess(this);
            process_server->start(program, arguments);

            qDebug() << "Launching server #" << i + 1 << endl;;
        }

        arguments.clear();
        arguments << QString::number(8000 + ui->spin_threads->value());
        process_server = new QProcess(this);
        process_server->start(program, arguments);

        qDebug() << "Launching server #" << ui->spin_threads->value() << endl;
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
        arguments << seed << ui->line_evolve->text() << evolution_filename;
        qDebug() << "arguments: " << arguments << endl;

        n_generation = 0;
        ui->list_generationsbest->clear();
        ui->progress_generation->setMaximum(ui->spin_generations->value());
        ui->progress_generation->setValue(0);
        process_evolve = new QProcess(this);
        process_evolve->start(program, arguments);
        connect(process_evolve, SIGNAL(finished(int)), this, SLOT(onEvolveFinished(int)));
        connect(process_evolve, SIGNAL(readyRead()), this, SLOT(evolveReadReady()) );
    }



}

void MainWindow::onPushSimulate()
{
    QString program = ui->edit_robogenpath->text() + "/robogen-file-viewer";
    QStringList arguments;
    QString path = ui->line_evolve->text() + "/" + ui->list_generationsbest->currentItem()->text();
    arguments << path << project_path + "/sim.txt";
    process_simulate = new QProcess(this);
    process_simulate->setWorkingDirectory(ui->edit_robogenpath->text());
    process_simulate->start(program, arguments);

    //qDebug() << process_simulate->readAll() << endl;

}

void MainWindow::onPushAnalyze()
{
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
}

void MainWindow::evolveReadReady()
{
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
}

void MainWindow::onEvolveFinished(int)
{
    ui->push_evolve->setEnabled(true);
}

PART_TYPE MainWindow::typeFromString(QString str)
{
    PART_TYPE type;

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

    return type;
}

MainWindow::~MainWindow()
{
    delete ui;
}
