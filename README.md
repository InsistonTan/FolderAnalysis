# FolderAnalysis
## 简介
本项目基于QT框架(QT6.x)开发, 实现磁盘占用分析, 通过扫描磁盘上的文件夹, 根据文件夹的总大小进行排序展示

---

## 怎么编译本项目
### 依赖
- QT6.x [QT官网](https://www.qt.io/)
- 开发工具 QT Creator 或者其它
### 编译
- clone或下载本项目源码
- QT Creator打开本项目的 FolderAnalysis.pro 文件
- 执行qmake
- 编译运行

## 已实现的功能
- 使用多线程扫描磁盘上的文件夹, 按照文件夹的总大小由大到小进行排序
- 支持当前路径跳转到系统资源管理器
- 支持文件多选批量删除


---

## License
This project is licensed under the [MIT License](LICENSE).
