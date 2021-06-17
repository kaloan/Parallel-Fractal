# Parallel-Fractal

A project for a university course on parallel processing. <br>

Doesn't look very pretty (~90s fractals), as that is a result of the colour function. More complicated methods are used for modern colouring of this type of fractals. <br>

The output is a .ppm file, essentially a bitmap. Since not a lot of programs may open it, you may need to convert it to another format. <br>

If you use GCC then change the scan functions at the start of main to the non \_s versions. <br>
Compiling with O2 is advised - speeds up by about x7 compared to unoptimized. Maybe because I haven't used SIMD (what was I thinking??!), but I not certain if that is the cause.
