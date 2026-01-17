# VCL-2025Fall-FinalProject
北京大学2025秋季-可视计算与交互概论期末大作业

## 本地配置方式
本项目可以通过 git 配置，由 xmake 构建。

``` bash
git clone https://github.com/lh314-pku/VCL-2025Fall-FinalProject.git
cd VCL-2025Fall-FinalProject && xmake
```

运行 `xmake run Final` 运行实验结果。

## 说明
xmake 会自动下载所有依赖项并编译，并将结果生成在 `build` 文件夹中。

本作业可以用作参考，是 [http://www.kevinbeason.com/smallpt/] 项目的复现与优化，加入了 BVH 进行更快的渲染。
