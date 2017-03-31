#ifndef ROBOTCONFIGFORM_H
#define ROBOTCONFIGFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "configurations.h"


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




private:
    Ui::RobotConfigForm *ui;
    PART_TYPE typeFromString(QString str);
    QString stringFromType(PART_TYPE tp);
};

#endif // ROBOTCONFIGFORM_H
