# CGILabs
This is an OpenGL Lab for students in the computer science department at ENSICAEN, an engineering scool in Caen, Normandy, France. They can of course, be used by any 
other students willing to learn OpenGL (although better tutorials probably exist elsewhere). The labs are associated with 3 lectures
introducing to the basics of OpenGL. The slides are available here:
* [Lecture 1](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/Cours/Lesson1)
* [Lecture 2](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/Cours/Lesson2)
* [Lecture 3](http://www.ecole.ensicaen.fr/~simonl/files/OpenGL/Cours/Lesson3)

# List of labs
* *fromScratch* : an empty project where to implement the first triangle (using the code snippets from the slides)
* *Minimal* : a gap fill project on Back Face Culling, parametric surfaces and depth buffer
* *Texture* : a gap fill project on texture mapping
* *Shading* : a gap fill project on classical illumination shaders (Lambert and Phong) as well as Normal Mapping
* *Mirror*  : a gap fill project on planar mirror (using the stencil buffer)
* *VShadow* : a gap fill project on Shadow Volumes (using the stencil buffer)
* *Game/RubikCube* : a simple Rubik's cube game, meant as an example for the final project.

# Structure of the labs
A part from the *fromScratch* tutorial which is a standalone lab, and the Rubik's cube, all other labs are structured identically. Each lab is decomposed into a set
of exercises, clearly marked out thanks to c++ comments of the form:
```
/* !todo exercise 1: Short instruction {{{
 * A longer (optional) paragraph describing the exercices in more details.
 *}}}*/
```
Besides, most of these exercise blocks are placed in *if* statements bound to a global variable state used to switch between the 
different exercises at run type (by typing 'E').

# Requirements
The project relies on the following dependencies
* Cmake 2.8
* OpenGL 3.2
* GLFW 3.0
* GLM 0.9.2

# Instructions
## Building
```bash
mkdir build
cd build
cmake ..
make
```
Note: You can add search paths via the standard Cmake variables.

## Running
All the labs, must be launched without any optional argument, from the build root (otherwise the assets/shaders will not be found). For example,
```bash
# running the minimal lab:
./Minimial/minimal
````

## Key bindings:
The following key bindings are available for all the labs:
* press 'h' to print this message again.
* press 'l' to toggle wireframe.
* press 'c' to toggle between camera and light controls
* press 'r' to reset the light or the camera to its initial position.
* press 'w/s' to move forward/backward :
* press 'a/d' to roll camera (rotation around Z):
* press 'Left/Right' to pan camera (rotation around Y):
* press 'Up/Down' to tilt camera (rotation around X):
* press 'e' to switch to the next exercise:
* press 'E' to switch to the previous  exercise.
* press an integer (in 0:maxExercise)  to switch to the corresponding  exercise.
* press 'q' or Esc to quit.

## Mouse controls
If the Picking functionalities are implemented, every object in the scene can be picked. By default, this object will be displayed
in wireframe. As an exception, in the *Vshadow* lab, the object will be selected as the shadow caster. 

# Author and License
The MIT License (MIT)

Copyright (c) 2016 Loic Simon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
Status API Training Shop Blog About Pricing
