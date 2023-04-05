#include <QApplication>
#include <QIcon>

#include "dataset.h"
#include "widget.h"

dataset totalset(5);  // 全局数据集对象，默认5比特，点击“运行”时才能最终确定

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Quantum Simulator - Deutsch Jozsa");
    w.setFixedSize(1100, 600);
    w.show();
    return a.exec();
}
