#include <QtWidgets>
#include <QPainter>
#include <QPushButton>
#include <QMessageBox>
#include <string.h>

#include "dataset.h"
#include "widget.h"
#include "ui_widget.h"
#include "resultform.h"
#include "ui_resultform.h"
#include "Python.h"

QPoint sourcepoint;  // 按下鼠标那一刻，选中的label的位置（备份label起始位置）
QString sourcename;  // 按下鼠标那一刻，选中的label的名字
QLabel* sourcelabel;  // 按下鼠标那一刻，选中的label

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setAcceptDrops(true);
    ui->setupUi(this);
    ui->Cnotgate->installEventFilter(this);  // 为工具栏的Cnot门设置右击事件
    ui->Swapgate->installEventFilter(this);  // 为工具栏的Swap门设置右击事件
}

Widget::~Widget()
{
    delete ui;
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void Widget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void Widget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset;

        QLabel *newIcon = new QLabel(this);
        labelrename(newIcon);  // 创建新label，立即重新命名
        newIcon->setPixmap(pixmap);

        QPoint droppoint = event->pos() - offset;  // 新label的落点

        if(checkin(sourcepoint.x(), sourcepoint.y()) && checkin_del(droppoint.x(), droppoint.y()))  // 删除元器件操作
        {
            newIcon->close();      
            int bitnum = transgetebit(sourcelabel);
            int ab = transpoint(sourcepoint.x(), sourcepoint.y());
            while(bitnum>0)
            {
                bitnum--;
                totalset.gate[ab%10 + bitnum][ab/10] = 0;  // 移动后把原位置的标志清空
                totalset.gate_type[ab%10 + bitnum][ab/10] = 1;  // 移动后把原位置的标志清空
            }
            sourcelabel->close();
            if(sourcelabel->objectName().contains("Measure"))
                totalset.MeasureGateNum--;
        }
        else
        {
            if(checkin(droppoint.x(), droppoint.y()))  // 如果label落点在操作区内
            {
                if(checkdrop(droppoint.x(), droppoint.y(), transgetebit(newIcon)) == true)  // 目标区域可以放置
                {
                    if(360 <= droppoint.x() && droppoint.x() <= 450)  // X坐标吸附
                        droppoint.setX(390);
                    else if (451 <= droppoint.x() && droppoint.x() <= 540)
                        droppoint.setX(490);
                    else if (541 <= droppoint.x() && droppoint.x() <= 630)
                        droppoint.setX(590);
                    else if (631 <= droppoint.x() && droppoint.x() <= 720)
                        droppoint.setX(690);
                    else if (721 <= droppoint.x() && droppoint.x() <= 810)
                        droppoint.setX(790);
                    else if (811 <= droppoint.x() && droppoint.x() <= 900)
                        droppoint.setX(890);

                    if(60 <= droppoint.y() && droppoint.y() <= 125)  // Y坐标吸附
                        droppoint.setY(80);
                    else if(126 <= droppoint.y() && droppoint.y() <= 215)
                        droppoint.setY(170);
                    else if(216 <= droppoint.y() && droppoint.y() <= 305)
                        droppoint.setY(260);
                    else if(306 <= droppoint.y() && droppoint.y() <= 395)
                        droppoint.setY(350);
                    else if(396 <= droppoint.y() && droppoint.y() <= 460)
                        droppoint.setY(440);

                    newIcon->move(droppoint);
                    newIcon->show();
                    newIcon->setAttribute(Qt::WA_DeleteOnClose);

                    if (checkin(sourcepoint.x(), sourcepoint.y())) {  // 在操作区中移动，仅作移动
                        int bitnum1 = transgetebit(newIcon);
                        int ab2 = transpoint(sourcepoint.x(), sourcepoint.y());
                        while(bitnum1>0)
                        {
                            bitnum1--;
                            totalset.gate[ab2%10 + bitnum1][ab2/10] = 0;  // 移动后把原位置的标志清空
                            totalset.gate_type[ab2%10 + bitnum1][ab2/10] = 1;  // 移动后把原位置的标志清空
                        }
                        event->setDropAction(Qt::MoveAction);
                        event->accept();
                    } else {  // 工具区移动到操作区的框，则复制
                        event->acceptProposedAction();
                        if(sourcename.contains("Measure"))
                            totalset.MeasureGateNum++;
                    }
                    int bitnum2 = transgetebit(newIcon);
                    int ab = transpoint(droppoint.x(), droppoint.y());
                    while(bitnum2>0)
                    {
                        bitnum2--;
                        totalset.gate[ab%10 + bitnum2][ab/10] = transgateindex(newIcon);  // 和ab2那行顺序不能反，必须先清除再设置
                        totalset.gate_type[ab%10 + bitnum2][ab/10] = transgetebit(newIcon);  // 和ab2那行顺序不能反，必须先清除再设置
                    }
                }
            }
        }
    } else {
        event->ignore();
    }
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));  // 获取鼠标当前指向的label
    if (!child)
        return;

    sourcelabel = child;  // 备份指向该label的指针
    sourcepoint = child->pos();  // 备份该label的初始位置
    sourcename = child->objectName();  // 备份该label的objectname

    QPixmap pixmap = *child->pixmap();  // 获取图片

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());  // 按下按钮的鼠标位置和label实际位置（左上角）的差值

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());  // 还原上方计算出来的差值（很细节，不然会出现label内漂移）

    QPixmap tempPixmap = pixmap;  // 画一个底纹（其实没必要，设置成透明就行了）
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 0));  // 拖拽的底色，设置为透明了
    painter.end();
    child->setPixmap(tempPixmap);

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    } else {
        child->show();
        child->setPixmap(pixmap);
    }
}

bool Widget::eventFilter(QObject *watched, QEvent *event)  // 为Uf门添加右击事件
{
    if(watched == ui->Cnotgate)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if(e->button()==Qt::RightButton)
            {
                totalset.cnottype = (totalset.cnottype + 1) % 4;

                if(totalset.cnottype == 0)  // 2比特
                {
                    QPixmap pixmap(":/Cnot2.png");
                    ui->Cnotgate->setPixmap(pixmap);
                    ui->Cnotgate->setFixedSize(80, 170);
                }
                else if(totalset.cnottype == 1)  // 3比特
                {
                    QPixmap pixmap(":/Cnot3.png");
                    ui->Cnotgate->setPixmap(pixmap);
                    ui->Cnotgate->setFixedSize(80, 260);
                }
                else if(totalset.cnottype == 2)  // 4比特
                {
                    QPixmap pixmap(":/Cnot4.png");
                    ui->Cnotgate->setPixmap(pixmap);
                    ui->Cnotgate->setFixedSize(80, 350);
                }
                else  // 5比特
                {
                    QPixmap pixmap(":/Cnot5.png");
                    ui->Cnotgate->setPixmap(pixmap);
                    ui->Cnotgate->setFixedSize(80, 440);
                }
            }
            emit sendObject(ui->Cnotgate);
        }
    }
    else if(watched == ui->Swapgate)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if(e->button()==Qt::RightButton)
            {
                totalset.swaptype = (totalset.swaptype + 1) % 4;

                if(totalset.swaptype == 0)  // 2比特
                {
                    QPixmap pixmap(":/Swap2.png");
                    ui->Swapgate->setPixmap(pixmap);
                    ui->Swapgate->setFixedSize(80, 170);
                }
                else if(totalset.swaptype == 1)  // 3比特
                {
                    QPixmap pixmap(":/Swap3.png");
                    ui->Swapgate->setPixmap(pixmap);
                    ui->Swapgate->setFixedSize(80, 260);
                }
                else if(totalset.swaptype == 2)  // 4比特
                {
                    QPixmap pixmap(":/Swap4.png");
                    ui->Swapgate->setPixmap(pixmap);
                    ui->Swapgate->setFixedSize(80, 350);
                }
                else  // 5比特
                {
                    QPixmap pixmap(":/Swap5.png");
                    ui->Swapgate->setPixmap(pixmap);
                    ui->Swapgate->setFixedSize(80, 440);
                }
            }
            emit sendObject(ui->Swapgate);
        }
    }
}

void Widget::on_qbit1_clicked()  // qbit1
{
    if(ui->qbit1->text() == "|0>")
    {
        ui->qbit1->setText("|1>");
        totalset.qbit[0] = totalset.qbit[0] * (-1);
    }
    else
    {
        ui->qbit1->setText("|0>");
        totalset.qbit[0] = totalset.qbit[0] * (-1);
    }
}

void Widget::on_qbit2_clicked()  // qbit2
{
    if(ui->qbit2->text() == "|0>")
    {
        ui->qbit2->setText("|1>");
        totalset.qbit[1] = totalset.qbit[1] * (-1);
    }
    else
    {
        ui->qbit2->setText("|0>");
        totalset.qbit[1] = totalset.qbit[1] * (-1);
    }
}

void Widget::on_qbit3_clicked()  // qbit3
{
    if(ui->qbit3->text() == "|0>")
    {
        ui->qbit3->setText("|1>");
        totalset.qbit[2] = totalset.qbit[2] * (-1);
    }
    else
    {
        ui->qbit3->setText("|0>");
        totalset.qbit[2] = totalset.qbit[2] * (-1);
    }
}

void Widget::on_qbit4_clicked()  // qbit4
{
    if(ui->qbit4->text() == "|0>")
    {
        ui->qbit4->setText("|1>");
        totalset.qbit[3] = totalset.qbit[3] * (-1);
    }
    else
    {
        ui->qbit4->setText("|0>");
        totalset.qbit[3] = totalset.qbit[3] * (-1);
    }
}

void Widget::on_qbit5_clicked()  // qbit5
{
    if(ui->qbit5->text() == "|0>")
    {
        ui->qbit5->setText("|1>");
        totalset.qbit[4] = totalset.qbit[4] * (-1);
    }
    else
    {
        ui->qbit5->setText("|0>");
        totalset.qbit[4] = totalset.qbit[4] * (-1);
    }
}

void Widget::on_Con_fun_clicked()  // 生成常函数
{
    totalset.realbit();  // 计算实际用到的比特数
    int f=0;
    double r=rand()/(RAND_MAX+1.0);  // 生成0-1随机数
    if(r-0.5>=0) f=0;
    else f=1;
    for(int i=0;i<totalset.pow2(2,totalset.n);i++)
        totalset.fx[i] = f;
    totalset.fxflag = false;
    QMessageBox::information(this,"提示","常函数已生成");
}

void Widget::on_Bala_fun_clicked()  // 生成平衡函数
{
    totalset.realbit();  // 计算实际用到的比特数
    int total = totalset.pow2(2, totalset.n);  // 2^n

    memset(totalset.fx,0,sizeof(totalset.fx));

    for(int i=0;i<total;i++)
    {
        if(i % 2 == 0)  // 偶数
            totalset.fx[i] = 1;
        else
            totalset.fx[i] = 0;
    }
    totalset.fxflag = true;
    QMessageBox::information(this,"提示","平衡函数已生成");
}

void Widget::on_Check_fun_clicked()  // 查看当前是什么函数
{
    if(totalset.fxflag == false)
        QMessageBox::information(this,"提示","当前f(x)是常函数");
    else
        QMessageBox::information(this,"提示","当前f(x)是平衡函数");

    // 应当输出f(x)的值和比特数n

}

void Widget::on_Do_clicked()
{
    totalset.realbit();  // 计算实际用到的比特数
    if(checkMeasure() == 1)  // 没有进行测量操作，非法
    {
        QMessageBox::information(this,"提示","必须包含测量操作");
        return;
    }
    else if(checkMeasure() == 2)  // 同一行多个测量门，非法
    {
        QMessageBox::information(this,"提示","不允许同一行多个测量门");
        return;
    }
    else if(checkMeasure() == 3)  // 测量门不在同一列，非法
    {
        QMessageBox::information(this,"提示","测量门不在同一列");
        return;
    }
    else if(checkMeasure() == 4)  // 前realbit行存在空行
    {
        QMessageBox::information(this,"提示","前 realbit 行存在空行");
        return;
    }
    else
    {
        /*=================================================================================================*/  // 将python需要的所有数据写入磁盘
        FILE* fp;  // 文件指针
        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\gate1.txt", "w")) == nullptr)  // 输出gate矩阵
            qDebug()<<"cannot open the gate1.txt!\n";
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<6;j++)
            {
                fprintf(fp,"%d ",totalset.gate[i][j]);
            }
            fprintf(fp,"\n");
        }
        fclose(fp);

        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\gate2.txt", "w")) == nullptr)  // 输出type表示的gate矩阵（方便Python计算）
            qDebug()<<"cannot open the gate2.txt!\n";
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<6;j++)
            {
                fprintf(fp,"%d ",totalset.gate_type[i][j]);
            }
            fprintf(fp,"\n");
        }
        fclose(fp);

        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\inistate.txt", "w")) == nullptr)  // 输出初始比特态
            qDebug()<<"cannot open the inistate.txt!\n";
        for(int i=0;i<totalset.n;i++)
        {
            if(totalset.qbit[i] == 1) fprintf(fp,"1 ");
            else fprintf(fp,"0 ");
        }
        fclose(fp);

        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\Measure.txt", "w")) == nullptr)  // 输出测量门位置
            qDebug()<<"cannot open the Measure.txt!\n";
        for(int i=0;i<totalset.n;i++)
        {
            fprintf(fp,"%d ", totalset.GateLine_1[i]);
        }
        fclose(fp);

        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\realbitnum.txt", "w")) == nullptr)  // 输出真实用到的比特数
            qDebug()<<"cannot open the realbitnum.txt!\n";
        fprintf(fp,"%d ",totalset.n);
        fclose(fp);

        /*=================================================================================================*/  // 调用python进行运算
        Py_SetPythonHome((const wchar_t *)(L"E://Anaconda-3.9"));  // python.exe路径
        Py_Initialize();
        if (!Py_IsInitialized())
            qDebug()<<"Can not initialized Python"<<endl;
        PyRun_SimpleString("import sys");//设置py脚本的路径
        PyRun_SimpleString("sys.path.append('./')");//当前路径
        PyObject* pModule = PyImport_ImportModule("opera");  // python文件名
        if (!pModule)
            qDebug()<< "Cant open python file!\n" << endl;
        PyObject* pFunhello= PyObject_GetAttrString(pModule,"opera");  // python文件中的入口函数
        if(!pFunhello)
            qDebug()<<"Get function opera() failed"<<endl;
        PyObject_CallFunction(pFunhello, nullptr);
        Py_Finalize();
        QMessageBox::information(this,"提示","已成功运行！");

        ResultForm *resultwindow = new ResultForm;  // 打开新窗口显示测量结果
        resultwindow->setWindowTitle("Result");
        resultwindow->setFixedSize(300, 450);
        resultwindow->show();
    }
}

bool checkin(int x,int y)  // 判断(x,y)是否在操作区内
{
    if ((360<=x && x<=900) && (60<=y && y<=460))
        return true;
    else
        return false;
}

bool checkin_del(int x, int y)  // 判断(x,y)是否在删除区内
{
    if ((1010<=x && x<=1200) && (440<=y && y<=600))
        return true;
    else
        return false;
}

bool checkdrop(int x, int y, int type)  // 判断类型为type的门是否可以放在(x,y)处，单比特type=1，以此类推
{
    int ab = transpoint(x, y);
    bool flag = true;
    while(type>0)  // 检查对应的列是否未放置门
    {
        type--;
        if(totalset.gate[ab%10+type][ab/10] != 0)  // 该位置已经放置了门
            flag = false;
    }
    return flag;
}

int transpoint(int x, int y)  // 将label的像素位置(x,y)转换成在gate数组中的坐标(a,b)
{
    int a=0, b=0;
    if(360 <= x && x <= 450)
        a=0;
    else if (451 <= x && x <= 540)
        a=1;
    else if (541 <= x && x <= 630)
        a=2;
    else if (631 <= x && x <= 720)
        a=3;
    else if (721 <= x && x <= 810)
        a=4;
    else if (811 <= x && x <= 900)
        a=5;

    if(60 <= y && y <= 125)
        b=0;
    else if(126 <= y && y <= 215)
        b=1;
    else if(216 <= y && y <= 305)
        b=2;
    else if(306 <= y && y <= 395)
        b=3;
    else if(396 <= y && y <= 460)
        b=4;
    return a*10+b;  // 以两位数形式返回两个值
}

int transgateindex(QLabel* temp)  // QLabel到门index的转换
{
    // H-1；S-2；T-3；X-4；Y-5；Z-6；Measure-7；Cnot-8；Swap-9
    if (temp->objectName().contains("Hgate"))
        return 1;
    else if (temp->objectName().contains("Sgate"))
        return 2;
    else if (temp->objectName().contains("Tgate"))
        return 3;
    else if (temp->objectName().contains("Xgate"))
        return 4;
    else if (temp->objectName().contains("Ygate"))
        return 5;
    else if (temp->objectName().contains("Zgate"))
        return 6;
    else if (temp->objectName().contains("Measuregate"))
        return 7;
    else if (temp->objectName().contains("Cnot"))
        return 8;
    else
        return 9;
}

int transgetebit(QLabel* temp)  // QLabel到门type的转换
{
    if (temp->objectName().contains("Hgate") || temp->objectName().contains("Sgate") || temp->objectName().contains("Tgate") ||temp->objectName().contains("Xgate") || temp->objectName().contains("Ygate") || temp->objectName().contains("Zgate") || temp->objectName().contains("Measuregate"))
        return 1;  // 单比特逻辑门
    else if(temp->objectName().contains("Cnot"))  // Cnot门
    {
        if(temp->objectName().contains("Cnot2gate"))
            return 2;
        else if(temp->objectName().contains("Cnot3gate"))
            return 3;
        else if(temp->objectName().contains("Cnot4gate"))
            return 4;
        else if(temp->objectName().contains("Cnot5gate"))
            return 5;
    }
    else if(temp->objectName().contains("Swap"))  // Cnot门
    {
        if(temp->objectName().contains("Swap2gate"))
            return 2;
        else if(temp->objectName().contains("Swap3gate"))
            return 3;
        else if(temp->objectName().contains("Swap4gate"))
            return 4;
        else if(temp->objectName().contains("Swap5gate"))
            return 5;
    }
    return 0;  // 啥门也不是
}

void labelrename(QLabel* temp)  // 为新标签重新命名
{
    if (sourcename.contains("Hgate"))
        temp->setObjectName("Hgate_"+ QString::number(totalset.gatenum[0]++));
    else if (sourcename.contains("Sgate"))
        temp->setObjectName("Sgate_"+ QString::number(totalset.gatenum[1]++));
    else if (sourcename.contains("Tgate"))
        temp->setObjectName("Tgate_"+ QString::number(totalset.gatenum[2]++));
    else if (sourcename.contains("Xgate"))
        temp->setObjectName("Xgate_"+ QString::number(totalset.gatenum[3]++));
    else if (sourcename.contains("Ygate"))
        temp->setObjectName("Ygate_"+ QString::number(totalset.gatenum[4]++));
    else if (sourcename.contains("Zgate"))
        temp->setObjectName("Zgate_"+ QString::number(totalset.gatenum[5]++));
    else if (sourcename.contains("Measuregate"))
        temp->setObjectName("Measuregate_"+ QString::number(totalset.gatenum[6]++));
    else if (sourcename.contains("Cnot"))  // 特殊考虑Cnot
    {
        if(sourcename == "Cnotgate")  // 工具栏里的
            temp->setObjectName("Cnot" + QString::number(totalset.cnottype + 2) + "gate_" + QString::number(totalset.gatenum[7]++));
        else
        {
            if (sourcename.contains("Cnot2gate"))
                temp->setObjectName("Cnot2gate_"+ QString::number(totalset.gatenum[7]++));
            else if (sourcename.contains("Uf3gate"))
                temp->setObjectName("Cnot3gate_"+ QString::number(totalset.gatenum[8]++));
            else if (sourcename.contains("Uf4gate"))
                temp->setObjectName("Cnot4gate_"+ QString::number(totalset.gatenum[9]++));
            else
                temp->setObjectName("Cnot5gate_"+ QString::number(totalset.gatenum[10]++));
        }
    }
    else  // 特殊考虑Swap
    {
        if(sourcename == "Swapgate")  // 工具栏里的
            temp->setObjectName("Swap" + QString::number(totalset.swaptype + 2) + "gate_" + QString::number(totalset.gatenum[11]++));
        else
        {
            if (sourcename.contains("Swap2gate"))
                temp->setObjectName("Swap2gate_"+ QString::number(totalset.gatenum[11]++));
            else if (sourcename.contains("Swap3gate"))
                temp->setObjectName("Swap3gate_"+ QString::number(totalset.gatenum[12]++));
            else if (sourcename.contains("Swap4gate"))
                temp->setObjectName("Swap4gate_"+ QString::number(totalset.gatenum[13]++));
            else
                temp->setObjectName("Swap5gate_"+ QString::number(totalset.gatenum[14]++));
        }
    }
}

int checkMeasure()  // 检查门是否合法，返回检查值：0-通过检查；1-测量门数量不足；2-同一行多个测量门；3-测量门不在同一列；4-前realbit行存在空行
{
    if(totalset.MeasureGateNum == 0) return 1;  // 测量门数量不足

    int MeasureGateLine[5] = {0};
    int MeasureGateRow[6] = {0};
    int GateLine[5] = {0};
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<6;j++)
        {
            if(totalset.gate[i][j]!=0)
                GateLine[i]++;
            if(totalset.gate[i][j] == 7)
            {
                MeasureGateLine[i]++;
                MeasureGateRow[j]++;
            }
        }
    }

    for(int i=0;i<totalset.n;i++)  // 前realbit行存在空行
        if(GateLine[i] == 0)
            return 4;

    for(int i=0;i<5;i++)  // 同一行多个测量门
    {
        if(MeasureGateLine[i]>1)
            return 2;
    }

    bool flag_row = false;
    for(int j=0;j<6;j++)
    {
        if(MeasureGateRow[j]!=0 && flag_row == false)
            flag_row = true;
        else if(MeasureGateRow[j]!=0 && flag_row == true)
            return 3;  // 测量门不在同一列
    }

    for(int i=0;i<5;i++)  // 拷贝到全局变量中
        totalset.GateLine_1[i] = MeasureGateLine[i];
    for(int j=0;j<6;j++)  // 拷贝到全局变量中
        totalset.GateRow_1[j] = MeasureGateRow[j];

    return 0;
}
