# Raytracer

A small weekend project to get used to C++ again. Not using much of the newer features of the language ;)
Code is adapted from 

https://raytracing.github.io/

and 

https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html

Current features:
- infinite plane, quad, sphere
- materials: lambert, metal, dielectric and diffuse-light
- multithreaded raytracing (# of threads = imageHeight / (numCores-2))
- NO optimizations :D
