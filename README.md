# FireDog
开源跨平台轻量级病毒特征检测引擎。Open source cross-platform lightweight virus signature detection engine.

# 语言 Language
C++ 11

# LICENSE

是你们喜欢的：MIT License.

让我们搞起来！

It's what you like:MIT License.

Let's do it!

# 交流 contact us

QQGroup: 337571436

Email: mountcloud@outlook.com

# 简介 Introduction

提供一个轻量级的特征检测引擎，支持自定义扩展特征库，等有时间再画引擎架构图。

Provide a lightweight feature detection engine, support custom extended feature libraries, and draw engine architecture diagrams when you have time.

# 特征库编辑器下载地址 Feature Library Editor download

https://github.com/MountCloud/FireDog/releases

# 更新日志 Update log


# v1.2.1

```
FireDog Version: v1.2.1
FireDog Editor Version: v1.0
Feature Libraray Version: v1.1.0

1：单资源匹配返回单条匹配特征改为返回多条匹配特征，这样单个文件允许被检测出多个特征。
2：【革命性更新】推出“特征库编辑器 FireDogEditor”，可以使用界面对特征库进行修改，并且进行测试，该编辑器支持国际化。

1: Single resource matching returns a single matching feature instead of returning multiple matching features, so that a single file allows multiple features to be detected.
2: [Revolutionary update] Launched the "feature library editor FireDogEditor", you can use the interface to modify and test the feature library, the editor supports internationalization.
```

## v1.2.1 界面GUI
### 特征库配置界面 Feature library configuration gui
![](images/v1.0-feature-library.png)

### 测试界面 Test gui
![](images/v1.0-matching.png)

### 设置语言界面 Language setting gui
![](images/v1.0-language.png)


# v1.1.0

```
1：重构特征库格式，特征库更加合理。
2：升级hex检测，支持通配符，例如：6D ?? ?5 6? [73-75] [41-5A,61-7A] 6C 6F 75 64
3：加入轻量级规则引擎/rule/rule.h，所以特征库支持使用规则进行条件匹配，语法类似mongodb的查询。

1: Refactor the format of the feature library to make the feature library more reasonable.
2: Upgrade hex detection to support wildcards, for example: 6D ?? ?5 6? [73-75] [41-5A,61-7A] 6C 6F 75 64
3: Add the lightweight rule engine /rule/rule.h, so the signature database supports the use of rules for condition matching, and the syntax is similar to mongodb query.
```

# v1.0.0 

```
支持hex、md5、text类型检测。
Support hex, md5, text type detection.
```

# 使用 Use

## Step 1: download
```
git clone https://github.com/MountCloud/FireDog.git
```
## Step 2: include
```
include FireDog
```
## Step 3: Example

```
look FireDogEditor/matchthread.h and FireDogEditor/matchthread.cpp
```
