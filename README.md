# Project of GPU Based Rendering: Birds. Created with CLion.
# 
## 

Birds
=====

![Project status][status]

Project of GPU Based Rendering 2017 @ ZJU.
Do parallel calculations with GLSL!
With use of image variable and FBO, it would be quite easy!

Table of Contents
-----------------

  * [Requirements](#requirements)
  * [Usage](#usage)
  * [Structure](#structure)
  * [Analysis](#analysis)
  * [Result](#result)
  * [Snapshot](#snapshot)
  * [License](#license)
  * [Contact](#contact)

Requirements
------------

1. 开发环境  
    1. Windows 7 SP1 64-bit  
    2. CLion 2017.3  
    3. MinGW 5.0  
    4. CMake 3.9.1
2. 硬件  
    NVIDIA Quadro K620 (OpenGL 4.5)
3. 第三方库  
    1. GLEW 7.0  
    2. GLUT 3.7  
    3. GLM 0.9.8.5

Usage
-----

本实验项目使用C++编写，在项目中依次包含GLEW, GLUT和GLM的头文件以及其他必要的系统头文件，使用CMake生成可执行文件。

For more information, please see [System Document][System Document]

Structure
-------------

```
Birds
├─documents
├─include
│  └─glm
│      ├─detail
│      ├─gtc
│      ├─gtx
│      └─simd
└─lib
```

Snapshot
--------
![][S0]

![][S1]

![][S2]

![][S3]

![][S4]

![][S5]

![][S6]

![][S7]

![][S8]

![][S9]

![][S10]

![][S11]

![][S12]

![][S13]

![][S14]

![][S15]


License
-------

PantyHouse is licensed under the [MIT][MIT] license.  
Copyright &copy; 2017, [Yunzhe][yunzhe].

Contact
-------

For any question, please mail to [yunzhe@zju.edu.cn][Mail]



[status]: https://img.shields.io/badge/status-finished-green.svg "Project Status: Finished"

[S0]: documents/0.png
[S1]: documents/1.png
[S2]: documents/2.png
[S3]: documents/3.png
[S4]: documents/4.png
[S5]: documents/5.png
[S6]: documents/6.png
[S7]: documents/7.png
[S8]: documents/8.png
[S9]: documents/9.png
[S10]: documents/10.png
[S11]: documents/11.png
[S12]: documents/12.png
[S13]: documents/13.png
[S14]: documents/14.png
[S15]: documents/15.png

[MIT]: /LICENCE_MIT.md
[yunzhe]: https://github.com/YunzheZJU

[Mail]: mailto:yunzhe@zju.edu.cn