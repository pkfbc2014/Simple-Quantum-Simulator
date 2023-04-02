#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <QDebug>

#include "dataset.h"
#include "widget.h"

dataset::dataset(int n)
{
    this->n = n;
    for(int i=0;i<5;i++)
        for(int j=0;j<6;j++)
            this->gate[i][j] = 0;
    for(int i=0;i<5;i++)
        this->qbit[i] = -1;
    for(int i=0;i<32;i++)  // 默认为常函数值是1
        this->fx[i] = 1;
    this->fxflag = false;
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
