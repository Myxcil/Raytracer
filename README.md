# Raytracer

A small weekend project to get used to C++ again. Not using much of the newer features of the language ;)
![Sample](/SampleImages/Screenshot%202023-04-16%20193847.jpg)

Code is adapted from 

https://raytracing.github.io/

and 

https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html

Current features:
- infinite plane, quad, sphere, box and trianglemesh (slow AF)
- materials: lambert, metal, dielectric and diffuse-light
- multithreaded raytracing, next unhandled scanline is pulled from vector
- NO optimizations :D
