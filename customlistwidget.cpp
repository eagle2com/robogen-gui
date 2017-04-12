#include "customlistwidget.h"
#include <QDebug>
#include <QKeyEvent>

CustomListWidget::CustomListWidget(QWidget *parent): QListWidget(parent)
{

}

void CustomListWidget::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Delete) {
        emit deletePressed();
    }
}
