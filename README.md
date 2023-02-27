# Moer-lite：面向教学的蒙特卡洛路径追踪渲染框架

## 简介

Moer-lite是一个面向教学的蒙特卡洛路径追踪框架。它提供了很多已有的功能，可以避免从零开始构建渲染器时遇到的一些工程性障碍。  
Moer-lite支持的功能不多，它只提供基础的积分方法、材质以及纹理，一些更强大的方法需要依靠同学们自己实现。  
Moer-lite的框架基于Moer主干：https://github.com/NJUCG/Moer .

这是一片自由发挥的乐园，希望同学们学习愉快。

## 编译和使用

请参考Lab0的编译使用过程。

## 示例场景
目前有三个示例场景：
- `examples/area-lights`：提供了一个面光源照亮一个Stanford bunny的场景；
- `examples/two-spotlights`：提供了一个有着两个点光源的场景；
- `examples/bunny`：提供了一个使用环境光（environment lighting）照亮Stanford bunny的场景。

## BUG反馈

如果发现Moer-lite框架存在bug，或者有任何架构上的改进意见，任何更好的第三方库……
请不要犹豫，你有多种渠道可以向我们反馈：
1. 直接在QQ上联系助教；
2. 向助教发邮件；
3. 在这个Github仓库的issues页面提出建议。

## TODO
- 对mesh和sphere实现表面采样（目前只有parallelogram可以配置为面光源）