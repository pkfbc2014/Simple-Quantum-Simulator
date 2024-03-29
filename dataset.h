#ifndef DATASET_H
#define DATASET_H

#include <QLabel>

class dataset  // 数据集，所有要用到的数据及操作
{
public:
    explicit dataset(int n);  // 自定义构造函数
    int pow2(int a,int b);  // 快速幂
    void realbit();  // 计算线路实际用到的比特数（按下Do按钮确定）

public:
    int n;  // 线路中实际比特数
    int qbit[5];  // 初始比特状态，默认为|0>，用-1表示|0>，这样按下按钮直接取反
    int gate[5][13];  // 操作区的门状态映射数组，目前最多5行13列，H-1；S-2；T-3；X-4；Y-5；Z-6；Measure-7；Cnot-8；Swap-9
    int gate_type[5][13];  //type表示的gate矩阵（方便Python计算）
    int fx[32];  // f(x)，最多2^5个
    int gatenum[15];  // 15个逻辑门数量（Cnot和Swap算四个），用于给label重命名
    int cnottype;  // Cnot门比特数：2bit-0；3bit-1；4bit-2；5bit-3
    int swaptype;  // Swap门比特数：2bit-0；3bit-1；4bit-2；5bit-3
    int MeasureGateNum;  // 测量门数量
    int GateLine_1[5];
    int GateRow_1[13];
    bool fxflag;  // 0-常函数，1-平衡函数
};

extern dataset totalset;  // 声明全局数据集对象

#endif // DATASET_H
