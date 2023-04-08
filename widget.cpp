#include <QtWidgets>
#include <QPainter>
#include <QPushButton>
#include <QMessageBox>
#include <string.h>

#include "dataset.h"
#include "widget.h"
#include "ui_widget.h"

QPoint sourcepoint;  // 按下鼠标那一刻，选中的label的位置（备份label起始位置）
QString sourcename;  // 按下鼠标那一刻，选中的label的名字
QLabel* sourcelabel;  // 按下鼠标那一刻，选中的label

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setAcceptDrops(true);
    ui->setupUi(this);
    ui->Ufgate->installEventFilter(this);  // 为工具栏的Uf门设置右击事件
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
            }
            sourcelabel->close();
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
                        }
                        event->setDropAction(Qt::MoveAction);
                        event->accept();
                    } else {  // 工具区移动到操作区的框，则复制
                        event->acceptProposedAction();
                    }
                    int bitnum2 = transgetebit(newIcon);
                    int ab = transpoint(droppoint.x(), droppoint.y());
                    while(bitnum2>0)
                    {
                        bitnum2--;
                        totalset.gate[ab%10 + bitnum2][ab/10] = transgateindex(newIcon);  // 和ab2那行顺序不能反，必须先清除再设置
                    }
                }
            }
        }
    } else {
        event->ignore();
    }

    for(int i=0;i<5;i++)  // 调试 - 输出映射矩阵gate
    {
        for(int j=0;j<6;j++)
        {
            qDebug()<<totalset.gate[i][j]<<" ";
        }
        qDebug()<<endl;
    }
    qDebug()<<"===============================";
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
    if(watched == ui->Ufgate)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            if(e->button()==Qt::RightButton)
            {
                totalset.uftype = (totalset.uftype + 1) % 4;
                if(totalset.uftype == 0)
                {
                    QPixmap pixmap(":/uf2.png");
                    ui->Ufgate->setPixmap(pixmap);
                    ui->Ufgate->setFixedSize(80, 170);
                }
                else if(totalset.uftype == 1)
                {
                    QPixmap pixmap(":/uf3.png");
                    ui->Ufgate->setPixmap(pixmap);
                    ui->Ufgate->setFixedSize(80, 260);
                }
                else if(totalset.uftype == 2)
                {
                    QPixmap pixmap(":/uf4.png");
                    ui->Ufgate->setPixmap(pixmap);
                    ui->Ufgate->setFixedSize(80, 350);
                }
                else
                {
                    QPixmap pixmap(":/uf5.png");
                    ui->Ufgate->setPixmap(pixmap);
                    ui->Ufgate->setFixedSize(80, 440);
                }
            }
            emit sendObject(ui->Ufgate);
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
    int total = totalset.pow2(2, totalset.n);  // 需要的随机数数量
    int flag = total;

    memset(totalset.fx,0,sizeof(totalset.fx));

    while(flag > total / 2)
    {
        int num = (rand() % (total - 0 + 1));   // 生成一个0-2^n随机数
        if(totalset.fx[num]==0){  // 其中total/2的地方变成1，若已经生成过，则再次生成一个，直到生成完毕
            totalset.fx[num] = 1;
            flag--;
        }
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
    // H-1；S-2；T-3；X-4；Y-5；Z-6；Measure-7；Uf-8（其余的）
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
    else return 8;  // Uf门默认为8
}

int transgetebit(QLabel* temp)  // QLabel到门type的转换
{
    if (temp->objectName().contains("Hgate") || temp->objectName().contains("Sgate") || temp->objectName().contains("Tgate") ||temp->objectName().contains("Xgate") || temp->objectName().contains("Ygate") || temp->objectName().contains("Zgate") || temp->objectName().contains("Measuregate"))
        return 1;  // 单比特逻辑门
    else if(temp->objectName().contains("Uf"))  // Uf门
    {
        if(temp->objectName().contains("Uf2gate"))
            return 2;
        else if(temp->objectName().contains("Uf3gate"))
            return 3;
        else if(temp->objectName().contains("Uf4gate"))
            return 4;
        else if(temp->objectName().contains("Uf5gate"))
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
    else  // 特殊考虑Uf
    {
        if(sourcename == "Ufgate")  // 从元器件栏拖出来的Uf
            temp->setObjectName("Uf" + QString::number(totalset.uftype + 2) + "gate_" + QString::number(totalset.gatenum[7]++));
        else
        {
            if (sourcename.contains("Uf2gate"))
                temp->setObjectName("Uf2gate_"+ QString::number(totalset.gatenum[7]++));
            else if (sourcename.contains("Uf3gate"))
                temp->setObjectName("Uf3gate_"+ QString::number(totalset.gatenum[8]++));
            else if (sourcename.contains("Uf4gate"))
                temp->setObjectName("Uf4gate_"+ QString::number(totalset.gatenum[9]++));
            else
                temp->setObjectName("Uf5gate_"+ QString::number(totalset.gatenum[10]++));
        }
    }
}
