#ifndef ROBOTCONFIGFORM_H
#define ROBOTCONFIGFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "configurations.h"

class MainWindow;

namespace Ui {
class RobotConfigForm;
}

class RobotConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit RobotConfigForm(QWidget *parent = 0);
    ~RobotConfigForm();
    QJsonObject getRobotJsonObject();
    QJsonObject getPartJsonObject(RobotPart* part);
    RobotConfiguration* current_config = nullptr;
    void loadAll();
    void saveAll();
    static QString stringFromType(PART_TYPE tp);
    static PART_TYPE typeFromString(const QString& str);
    void writeRobot(const QString& project_path);

private slots:
    void onAddPart();
    void onRemovePart();
    void onRobotPartChange(int index);
    void onRobotFaceChange(int index);
    void onRobotPartNameChange(QString name);
    void onRobotRotationChange(int index);
    void onRobotParamLengthChange(double value);
    void onRobotParamRotationChange(int value);
    void onItemChange(QTreeWidgetItem *, QTreeWidgetItem *);

    void onLoadRobot();
    void loadRobotJson(const QString& filename);



public:
    Ui::RobotConfigForm *ui;


};

#endif // ROBOTCONFIGFORM_H
