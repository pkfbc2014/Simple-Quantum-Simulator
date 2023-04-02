#ifndef DATASET_H
#define DATASET_H

#include <QLabel>

class dataset  // 数据集，所有要用到的数据及操作
{
public:
    explicit dataset(int n);  // 自定义构造函数
    int pow2(int a,int b);  // 快速幂

public:
    int n;  // 线路中实际比特数
    int qbit[5];  // 初始比特状态，默认为|0>，用-1表示|0>，这样按下按钮直接取反
    int gate[5][6];  // 操作区的门状态映射数组，目前最多5行6列，H-1；S-2；T-3；X-4；Y-5；Z-6；Measure-7；Uf-8
    int fx[32];  // f(x)，最多2^5个
    bool fxflag;  // 0-常函数，1-平衡函数
};

extern dataset totalset;  // 声明全局数据集对象

#endif // DATASET_H
