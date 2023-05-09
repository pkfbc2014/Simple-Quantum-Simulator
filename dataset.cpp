#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <QDebug>

#include "dataset.h"
#include "widget.h"

dataset::dataset(int n)  // 初始化
{
    this->n = n;
    this->fxflag = false;
    this->cnottype = 0;
    this->swaptype = 0;
    this->MeasureGateNum = 0;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<13;j++)
        {
            this->gate[i][j] = 0;
            this->gate_type[i][j] = 1;  // 空白处看成单位阵，单位阵是1bit的
        }
    }
    for(int i=0;i<5;i++)
        this->qbit[i] = -1;
    for(int i=0;i<32;i++)  // 默认为常函数值是1
        this->fx[i] = 1;
    for(int i=0;i<15;i++)
        this->gatenum[i] = 0;
}

int dataset::pow2(int a,int b)  // 快速幂
{
    int r=1,base=a;
    while(b!=0)
    {
        if(b%2) r*=base;
        base*=base;
        b/=2;
    }
    return r;
}

void dataset::realbit()  // 计算线路实际用到的比特数（按下Do按钮确定）
{
    int count = 0;
    for(int i=0;i<5;i++)
    {
        bool flag = false;
        for(int j=0;j<6;j++)
        {
            if(this->gate[i][j] != 0)  // 这一行有一个门，则说明使用到了这个比特
            {
                flag = true;
                break;
            }
        }
        if(flag == true) count++;
    }
    this->n = count;
}
