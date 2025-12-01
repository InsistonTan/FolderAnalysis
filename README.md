# FolderAnalysis
![GitHub_Stars](https://img.shields.io/github/stars/InsistonTan/FolderAnalysis?style=flat&color=ffcf49)
[![GitHub All Releases](https://img.shields.io/github/downloads/InsistonTan/FolderAnalysis/total)](https://github.com/InsistonTan/KeyMappingsTool/releases)
[![GitHub issues by-label](https://img.shields.io/github/issues/InsistonTan/FolderAnalysis?color=blue)](https://github.com/InsistonTan/KeyMappingsTool/issues)

硬盘存储占用分析工具

## 简介
本项目基于QT框架(QT6.x)开发, 实现硬盘存储占用分析, 通过扫描硬盘上的文件夹和文件, 根据文件夹和文件的总大小进行倒序展示. 还支持保存当前分析结果, 可以与已保存的结果进行对比分析 

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
- 支持当前路径跳转到系统文件资源管理器
- 支持文件多选批量删除
- 支持保存当前分析结果`在全盘扫描分析结束后, 点击右上角的 "保存当前分析结果" 按钮进行保存`
- 支持选择已保存的结果进行对比分析 `在扫描分析结束后, 点击 "对比历史分析结果" 按钮, 选择想要对比的历史分析结果文件`


---

## License
This project is licensed under the [MIT License](LICENSE).
