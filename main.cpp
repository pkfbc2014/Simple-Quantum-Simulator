#include <QApplication>
#include <QIcon>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <QDebug>
#include <qdesktopwidget.h>

#include "dataset.h"
#include "widget.h"
#include "Python.h"

dataset totalset(5);  // 全局数据集对象，默认5比特，点击“运行”时才能最终确定

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDesktopWidget *desktop = QApplication::desktop();
    Widget w;
    w.setWindowTitle("Quantum Simulator - Deutsch Jozsa");
    w.setFixedSize(1800, 600);
    w.move((desktop->width() - w.width())/ 2, (desktop->height() - w.height()) /3);  // 让窗口显示在1/3处比较舒服
    w.show();
    return a.exec();
}
