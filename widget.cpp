#include <QtWidgets>
#include <QPainter>
#include <QPushButton>
#include <QMessageBox>
#include <string.h>

#include "dataset.h"
#include "widget.h"
#include "ui_widget.h"

QPoint sourcepoint;  // 按下鼠标那一刻，选中的label的位置（备份label起始位置）

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setAcceptDrops(true);
    ui->setupUi(this);
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
        newIcon->setPixmap(pixmap);
        QPoint droppoint = event->pos() - offset;  // 新label的落点

        if(checkin(droppoint.x(), droppoint.y()))  // 如果label落点在操作区内
        {
            if(260 <= droppoint.x() && droppoint.x() <= 350)  // X坐标吸附
                droppoint.setX(290);
            else if (351 <= droppoint.x() && droppoint.x() <= 440)
                droppoint.setX(390);
            else if (441 <= droppoint.x() && droppoint.x() <= 530)
                droppoint.setX(490);
            else if (531 <= droppoint.x() && droppoint.x() <= 620)
                droppoint.setX(590);
            else if (621 <= droppoint.x() && droppoint.x() <= 710)
                droppoint.setX(690);
            else if (711 <= droppoint.x() && droppoint.x() <= 800)
                droppoint.setX(790);

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

            if (checkin(sourcepoint.x(), sourcepoint.y())) {  // 在自己的框中移动，仅作移动
                event->setDropAction(Qt::MoveAction);
                event->accept();
            } else {  // 移动到别的框，则复制
                event->acceptProposedAction();
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

    sourcepoint = child->pos();  // 备份该label的初始位置

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

}

bool checkin(int x,int y)  // 判断(x,y)是否在操作区内
{
    if ((260<=x && x<=800) && (60<=y && y<=460))
        return true;
    else
        return false;
}

// qDebug()<<"单击了按钮"<<endl;
