#include <QDebug>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "resultform.h"
#include "ui_resultform.h"
#include "dataset.h"
#include "widget.h"

ResultForm::ResultForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultForm)
{
    ui->setupUi(this);
}

void ResultForm::LoadData()  // 加载测量结果到文本框中
{
    FILE* fp;
    int filesize = 0;  // 文件长度
    if(totalset.MeasureGateNum == 0)  // 不包含测量操作，输出振幅
    {
        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\MiddleState.txt", "r")) == nullptr)
            qDebug()<<"cannot open the MiddleState.txt!\n";
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);  // 先单独获取一遍文件长度，再打开文件
        fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\MiddleState.txt", "r");
        ui->windowtitle->setText("振幅信息");
    }
    else  // 输出测量结果
    {
        if ((fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\MeasureResult.txt", "r")) == nullptr)
            qDebug()<<"cannot open the MeasureResult.txt!\n";
        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);  // 先单独获取一遍文件长度，再打开文件
        fp = fopen("C:\\Users\\14768\\Desktop\\DataCache\\MeasureResult.txt", "r");
    }
    char str[2000];
    fgets(str, filesize, fp);
    for(int i=0;i<strlen(str);i++)
    {
        if(str[i] == 'k') str[i] = '\n';
        if(str[i] == 'o') str[i] = '\t';
    }

    fclose(fp);
    ui->Datalabel->setText(str);
}

ResultForm::~ResultForm()
{
    delete ui;
}
