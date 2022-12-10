# GPU volume-rendering


## How to run

The application runs on Microsoft Windows operation systems.
You can run by clicking the executable.

You can start from command line;
This way you can specify the foldername of the volume data:

`x64/Release/HomebrewGraphicsEngine.exe <folder name>`

The folder must be placed under `<project directory>/Resources/Textures/`

It must contain the slice images of the volume named like `###.tif` and a `dimensions.txt` file.

The `dimensions.txt` file must have the following content:

`name = <name of the dataset>`

`width = <int>`

`height = <int>`

`depth = <int>`

`bytesPerVoxel = <int>`

`widthScale = <float>`

`heightScale = <float>`

`depthScale = <float>`


We provide you some sample dataset:

`cthead-8bit`


<hr>

## User controls


Toggle options:		`O key`

Toggle display transfer-function: `H key`

Toggle ray-cast / half-angle slicing: `M key`

Reload shaders: `R key`

Move slicing plane: `C key + Left mouse button on bounding box + drag`

Rotate camera: `Right mouse button`

Zoom camera: `Right mouse button`

Turn light source: `= key / - key`

Switch between transfer function features: `Space`

Toggle fullscreen: `Tab`


<hr>

## The application is powered by HomebrewGraphicsEngine
Created by Zoltán Simon

A grahics/videogame engine writen in C++ using OpenGL.

- Collision and physics engine
- Deferred PBR shading with bloom
- Instanced rendering
- Font rendering
- Audio

![HoGraEngine_logo](Resources/Icons/HoGraEngineLogo.png "HoGraEngine logo")

<hr>

<a href="https://www.flaticon.com/free-icons/idea" title="idea icons">Idea icons created by Good Ware - Flaticon</a>
