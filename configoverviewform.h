#ifndef CONFIGOVERVIEWFORM_H
#define CONFIGOVERVIEWFORM_H

#include <QWidget>
#include "configurations.h"

namespace Ui {
class ConfigOverviewForm;
}

class ConfigOverviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigOverviewForm(QWidget *parent = 0);
    ~ConfigOverviewForm();
    ProjectConfiguration* current_config = nullptr;
    void loadAll();
    void saveAll();

private slots:
    void onNameChange();


private:
    Ui::ConfigOverviewForm *ui;
};

#endif // CONFIGOVERVIEWFORM_H
