#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QProcess>
#include <QHash>

enum class PART_TYPE {CORE_COMPONENT, FIXED_BRICK, ACTIVE_HINGE, PASSIVE_HINGE, PARAMETRIC_JOINT, LIGHT_SENSOR, IR_SENSOR};
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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onAddPart();
    void onRemovePart();
    void onItemChange(QTreeWidgetItem *, QTreeWidgetItem *);
    void onRobotPartChange(int index);
    void onRobotFaceChange(int index);
    void onRobotPartNameChange(QString name);
    void onRobotRotationChange(int index);
    void onRobotParamLengthChange(double value);
    void onRobotParamRotationChange(int value);

    void loadSimulation();
    void loadEvolution();
    void loadRobot();
    void loadRobotJson(QString filename);

    void onLoadConfig();
    void loadConfig(QString default_path = "");

    void saveSimulation();
    void saveEvolution();
    void saveRobot();

    void saveAll();

    void browseRobogen();
    void browseEvolve();
    void browseCustomScenario();

    void onPushEvolve();
    void onPushSimulate();
    void onPushAnalyze();
    void onPushStop();

    void evolveReadReady();
    void onEvolveFinished(int);

private:
    Ui::MainWindow *ui;

    RobotPart *root_part;
    QString project_path;

    QProcess *process_server;
    QProcess *process_evolve;
    QProcess *process_simulate;

    QHash<QString, RobotPart*> robot_part_hash;

    PART_TYPE typeFromString(QString str);

    int n_generation;
};

#endif // MAINWINDOW_H
