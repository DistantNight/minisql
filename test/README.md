# 测试
测试用例可以放在这个文件夹。测试用例中一般都包含 `main` 函数，使用时注意项目中应只有一个测试用例。
## 约定

- 全局变量 `database` 与 `catalog_manager`

  ```c++
  /* Other include */
  #include "InputProcess.h"
  #include "catalog.h"
  #include "buffer.h"
  
  catalogManager *catalog_manager = nullptr;
  Buffer* database = nullptr;
  int main()
  {
      Buffer d;
      database = &d;
      catalogManager c;
      catalog_manager = &c;
  
  }
  ```

  Q：为什么这两个全局变量要是指针？ A：C++ 的创造者给出的[编程规范](https://docs.microsoft.com/zh-cn/visualstudio/code-quality/c26426?f1url=https%3A%2F%2Fmsdn.microsoft.com%2Fquery%2Fdev15.query%3FappId%3DDev15IDEF1%26l%3DZH-CN%26k%3Dk(C26426)%26rd%3Dtrue)中说要避免复杂全局对象的初始化 
