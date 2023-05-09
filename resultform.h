#ifndef RESULTFORM_H
#define RESULTFORM_H

#include <QWidget>

namespace Ui {
class ResultForm;
}

class ResultForm : public QWidget
{
    Q_OBJECT

public:
    explicit ResultForm(QWidget *parent = nullptr);
    void LoadData();  // 载入测量结果到文本框中
    ~ResultForm();

private:
    Ui::ResultForm *ui;
};

#endif // RESULTFORM_H
