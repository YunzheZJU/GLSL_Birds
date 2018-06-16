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

Structure
-------------

```
Birds
├─documents
├─executable        // Pre-built executable files for Windows
├─include
│  └─glm
│      ├─detail
│      ├─gtc
│      ├─gtx
│      └─simd
└─lib
```

Analysis
--------

1. Overall：实验效果  
    本次实验的最终效果如下图所示。在不干扰的情况下鸟群应沿环形轨道绕中心盘旋。
    
    ![][S0]

2. 个体运动算法介绍

    ![][S1]
    <center>速度的更新过程</center>

    ![][S2]
    <center>子流程“避让/亲近”</center>

    鸟群的位置和速度信息分别存放于两张纹理图片中，每次更新时（每秒约60次）读入旧的位置和速度信息，更新每一只鸟的位置和速度信息，再将新的位置应用于每一只鸟身上，重新绘制每一个顶点的位置。此节中将着重介绍更新过程中每一个个体所表现出的避让、附和和亲近过程，而忽略OpenGL和GLSL端的各种设置过程。
    如Figure 1和Figure 2所示，每一只鸟在更新速度信息时，都会在以自身为中心的一定范围r内寻找其他鸟，并根据它与自身的距离d与最大距离r的比值persent的处于哪一区域内，做出相应的判断：易知percent的大小处于(0, 1)，我们设定一个“独立阈值”（如0.45）和“对齐阈值”（如0.65），则当前鸟与目标鸟的位置关系可能有三种：小于“独立阈值”；大于等于“独立阈值”小于“对齐阈值”；大于等于“对齐阈值”。
    1. 小于“独立阈值”。此时两鸟的间距被判定为过小，当前鸟的在自身速度的基础上增加一个反向避让的速度。由于速度的大小会被最后的速度限制所处理，所以主要作用是影响速度方向。
    ```c
    if (percent < separationThresh) { // low
        // 低于separation阈值，靠得太近了
        // Separation - Move apart for comfort
        f = (separationThresh / percent - 1.0) * delta;
        velocity -= normalize(dir) * f;
    ```
    2. 大于等于“独立阈值”小于“对齐阈值”。在这个位置范围内的目标鸟将被当前鸟作为“对齐榜样”，目标鸟的速度矢量将被加成到当前鸟的速度上，造成的影响是使得当前鸟倾向于与目标鸟保持同一方向移动。
    ```c
    } else if (percent < alignmentThresh) { // high
        // 低于alignment阈值（separation的值也有影响）
        // Alignment - fly the same direction
        float threshDelta = alignmentThresh - separationThresh;
        float adjustedPercent = (percent - separationThresh) / threshDelta;
        // 取出参照鸟的速度，把参照鸟的速度方向加到自己身上
        birdVelocity = getVelocity(ref).xyz;
        f = (0.5 - cos(adjustedPercent * PI_2) * 0.5 + 0.5) * delta;
        velocity += normalize(birdVelocity) * f;
    ```
    3. 大于等于“对齐阈值”。目标鸟处于一个不太远又不是很近的距离，我们让当前鸟的速度增加一个指向目标鸟的速度矢量，使它们靠得更近些以进入到“对齐阈值”的范围之内。
    ```c
    } else {
        // 否则：靠得有点远
        // Attraction / Cohesion - move closer
        float threshDelta = 1.0 - alignmentThresh;
        float adjustedPercent = (percent - alignmentThresh) / threshDelta;
    
        f = (0.5 - (cos(adjustedPercent * PI_2) * -0.5 + 0.5)) * delta;
    
        velocity += normalize(dir) * f;
    }
    ```
    而在实际应用过程中，我设置了三个值，分别为Seperation Distance、Alignment Distance和Cohesion Distance。独立阈值separationThresh和对齐阈值alignmentThresh的计算方法如Figure 6所示，zoneRadius即上文提到的以自身为中心的半径r，这保证了在Seperation Distance和Alignment Distance都大于等于0 的情况下，对齐阈值始终大于等于独立阈值。
    ```c
    separationThresh = seperationDistance / zoneRadius;
    alignmentThresh = (seperationDistance + alignmentDistance) / zoneRadius;
    ```
    仅此，一个个体就可以表现出一定的行为。在大一些的尺度来看，已经可以表现出一定的群体效果。我们最后为它加上一个对“捕食者”的躲避行为：在窗口中移动鼠标，每一只鸟将计算鼠标位置在空间中对应的坐标与自身的距离，若“捕食者”进入了自己的“索敌”范围，则进行避让。于是在窗口中移动鼠标可以干扰鸟群，产生有趣的效果。
    ```c
    // dir是捕食者相对当前鸟的位置
    dir = predator * BOUNDS - selfPosition;
    dir.z = 0.;
    // 与捕食者的距离
    dist = length(dir);
    distSquared = dist * dist;

    // 寻找捕食者的距离
    float preyRadius = 100.0;
    float preyRadiusSq = preyRadius * preyRadius;

    // 与捕食者距离过远不会引发躲避行为
    if (dist < preyRadius) {
        // move birds away from predator
        // 躲避速度的大小
        f = (distSquared / preyRadiusSq - 1.0) * delta * 100.;
        // 躲避方向为远离捕食者方向
        velocity += normalize(dir) * f;
        // 增加速度上限
        limit += 5.0;
    }
    ```
3. 使用纹理、FBO和片段着色器更新位置和速度
    1. 尝试使用OpenGL的image variable，允许片段着色器对传入的纹理进行直接读写。
    2. “乒乓渲染”，是一种常用的渲染技巧。结合image变量，我生成了一张很大的纹理（如若有32*32只鸟，则创建一张64*64的纹理），分为4份，分别存储位置数据、速度数据和第二份位置和速度数据，每次更新都是在同一张纹理的不同部分进行操作。
    3. iii.	使用了扩展的GL_RGBA32F颜色格式，为提高计算精度。其实16F已经足够。

Result
------

1. 总体效果  
    实验总体效果如图所示。在不干扰的情况下鸟群应沿环形轨道绕中心盘旋。
    
    ![][S0]
    
2. 控制：暂停与恢复  
    按下空格或将焦点切换到其他窗口时，计算会暂停。
    
    ![][S3]
    
3. 控制：干扰  
    鼠标移入窗口，对鸟群产生干扰。
    
    ![][S4]
    
4. 观察：改变颜色  
    按下R键，可以切换鸟个体的颜色为随机颜色还是根据速度方向创建的颜色：若速度在Z轴的分量指向+Z，则颜色偏向红色，否则偏向蓝色。
    
    ![][S5]
    
5. 观察：改变视角  
    使用WSADQE键进行环绕和缩放观察，操作起来需要一段时间适应。按C进入第一人称漫游模式。再次按C恢复初始视角。请注意任何细小的视角操作都会影响(3)和(4)的效果，请尽可能在初始视角中使用以上功能。
    
    ![][S6]
    
6. 观察：截图  
    按X键保存当前窗口截图于运行目录。
    
    ![][S7]
    
7. 参数：创建鸟的数量  
    可以创建的鸟的数量需为2的指数级的平方只，如输入32，则会创建32 * 32 = 1024只。
    
    ![][S8]
    
8. 参数：Seperation Distance  
    使用方向键的上↑和下↓控制Seperation Distance的大小。更大的值使得个体间的间距增加。
    
    ![][S9]
    
9. 参数：Alignment Distance  
    使用方向键的左←和右→控制Alignment Distance的大小。更大的值使得群体的方向统一性增加。
    
    ![][S10]
    
10. 参数：Cohesion Distance  
    使用Home和End键控制Cohesion Distance的大小，更大的值将减弱前面两者的影响。
    
    ![][S11]

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

[MIT]: /LICENCE_MIT.md
[yunzhe]: https://github.com/YunzheZJU

[Mail]: mailto:yunzhe@zju.edu.cn