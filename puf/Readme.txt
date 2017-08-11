主要接口文件是enroll.c和rep.c。
1. enroll.c是登记阶段，用于产生一个被保护的随机数PS，然后使用BCH纠错码计算产生不需保护但是需要存储的code；
2. rep.c是重构阶段，利用登记阶段的code重构出PS。
