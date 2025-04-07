# Path Tracer in C
This is a simple path tracer written in C. It doesn't need any external libraries to run. Simply compiling the main.c file with any C compiler (I used gcc) will allow you to get the executable and run the program. The images that will be generated are going to be in a .ppm format. The images needed for the textures are in the .ppm format as well (specifically P6). 

For now, this path tracer can render spheres and meshes. I do not plan on working further on this version of the path tracer, since writing a complex path tracer in C is starting to become exhaustive due to the lack of things like classes and such. I will rewrite the project from scratch in C++.

Known issues: 
- I still don't know why, but the code that finds the position of each pixels in the world is incorrect, but I plan on fixing it in the C++ version. For now, if the camera's center remains at (0, 0, 0), it's target stays at (0, 0, -1) and it's up stays at (0, 1, 0) with a full vertical fov of 60, then the images generated will look great!
- I think the sphere uv coordinates aren't being found properly, so textures on spheres might look weird.

This project was a great learning experience for me. I've always wanted to learn a little bit of C and this was the perfect way to to so. I obviously haven't delved SUPER deep in the langage's features, but I am overall satisfied with my progress in the language. I also wanted to extend my knowledge about ray tracers and building a path tracer was a fun experience!
