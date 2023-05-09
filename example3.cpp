#include "example3.h"
#include "ui_example3.h"

example3::example3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::example3)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);  // 只读，不允许编辑
}

example3::~example3()
{
    delete ui;
}
