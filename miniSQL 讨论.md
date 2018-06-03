# miniSQL 讨论

## 时间地点

- 2018年6月3日 19：00
- 碧峰 2 楼下

## 人员

全体

## 议程

1. 选择编程语言：C++

2. 代码风格约定：

   - 大括号格式：换行
   - 缩进格式：4 个空格
   - 变量命名方式：`small_varible`
   - 函数命名方式：`camalCase`
   - 类的命名方式：`ThisIsClass`

3. 讨论架构设计：40 分钟

   架构设计

   - 确定各个构造块的责任：30 分钟

     - Interpreter 检查语法性的错误，产生`class Operation`，与 catalog manager 交互，根据 API 返回值显示查询结果
     - API 根据 `class Operation` 调用各种 manager 的接口

   - 确定各个构造块的通信规则：TODO

   - 确定主要的类：TODO: 负责 API 的人需要定义通信类

     ```c++
     class Operation {}; // for interpreter 
     class Table {}; // catalog manager, interpreter

     ```

   - 确定错误处理方式：使用异常（参考 C++ Primer 18.1）


4. 讨论分工方式：

   - API and Interpreter：范佳晨
   - Buffer Manager and DB Files: 曾辉
   - Index Manager and record manager: 闫徐天任
   - Catalog Manager：吕明远
   - 整合：赵丰年

5. 约定集成工序：自己进行单元测试，文件直接提交到 master 分支

   ​
