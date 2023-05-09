#include "example1.h"
#include "ui_example1.h"

example1::example1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::example1)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);  // 只读，不允许编辑
}

example1::~example1()
{
    delete ui;
}
