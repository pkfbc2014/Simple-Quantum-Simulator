#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void sendObject(QObject *t);

private slots:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_qbit1_clicked();
    void on_qbit2_clicked();
    void on_qbit3_clicked();
    void on_qbit4_clicked();
    void on_qbit5_clicked();
    void on_Con_fun_clicked();
    void on_Bala_fun_clicked();
    void on_Check_fun_clicked();
    void on_Do_clicked();

private:
    Ui::Widget *ui;
};

extern QPoint sourcepoint;  // 按下鼠标那一刻，选中的label的位置（备份label起始位置）
extern QString sourcename;  // 按下鼠标那一刻，选中的label的名字
extern QLabel* sourcelabel;  // 按下鼠标那一刻，选中的label

bool checkin(int x,int y);  // 判断是否在操作区内
bool checkin_del(int x, int y);  // 判断(x,y)是否在删除区内
bool checkdrop(int x, int y, int type);  // 判断类型为type的门是否可以放在(x,y)处，单比特type=1，以此类推
int transpoint(int x, int y);  // 将label的像素位置(x,y)转换成在gate数组中的坐标(a,b)
int transgateindex(QLabel* temp);  // QLabel到门编号的转换
int transgetebit(QLabel* temp);  // QLabel到门type的转换
void labelrename(QLabel* temp);  // 为新标签重新命名
int checkMeasure();  // 检查门是否合法，返回检查值

#endif // WIDGET_H
