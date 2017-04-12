#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>

class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    CustomListWidget(QWidget* parent = 0);
    void keyPressEvent(QKeyEvent* ev);

signals:
    void deletePressed();
};

#endif // CUSTOMLISTWIDGET_H
