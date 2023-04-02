#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

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

bool checkin(int x,int y);  // 判断是否在操作区内

#endif // WIDGET_H
