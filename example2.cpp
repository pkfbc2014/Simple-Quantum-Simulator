#include "example2.h"
#include "ui_example2.h"

example2::example2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::example2)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);  // 只读，不允许编辑
}

example2::~example2()
{
    delete ui;
}
