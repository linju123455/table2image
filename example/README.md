## 例程
本例程使用到的json解析均为测试，使用者可直接根据实际场景对结构体进行赋值即可

## 编译
```shell
rm -rf build/
mkdir build
cd build
cmake ..
make -j4
```

## 性能分析
120*30的9个单元格组成的表格，DrawTable耗时2-3ms，AdjustTable耗时0-1ms
