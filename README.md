# Fractalism
Fractalism is a unique fractal visualization tool with support for iteration animation
and multiple hypercomplex number systems.

## Motivation
I wanted to know more about fractals using different number systems. I noticed a lack
of mathematical rigor in publically-avalible fractal rendering programs, and none that
I could find supported multicomplex number systems. I was also very curious how each
point "moved" with each iteration, and could find no visualizations of that anywhere.

Most of my background has been Enterprise Java development, but my personal computer
is rather outdated (it was made in 2014), and I needed to be able to squeeze as much
performace out of it as possible, so I taught myself enough C++ to write this program.

## User Guide
The user interface is divided into 5 windows:
1. **Control Panel**: This section (displayed on the right side of the screen by default)
is used to control render settings that are shared between all the render windows:
    1. **Parameter**: This hypercomplex number is passed to the *dynamical space*
    rendering windows.
    2. **Number System**: The hypercomplex number system to use for the fractal calculations.
    Currently, only *complex*, *bicomplex*, and *quaternion* are supported.
    3. **Dimensions**: The render dimensions. Either *2D* or *3D*.
    4. **Resolution**: The size of the textures that the render windows use.[^1]
2. **Phase escape**: Displays a static phase-space[^2] fractal using the classic
fractional escape value.[^3]
3. **Phase translated**: Displays an animated phase-space fractal. Each pixel (or
voxel in 3D mode) is initially assigned a unique color value[^4] that represents
the initial value of that point (which is the point itself). As each iteration is
calculated, the position of the point is translated to the point on the texture
that corresponds to the new value. This allows us to see how the values change over
successive iterations, rather than focusing exclusively on the behaviour as the
iteration approaches infinity.
4. **Dynamical escape**: Same as *Phase escape*, but in the dynamical-plane.[^5]
5. **Dynamical translated**: Same as *Phase translated*, but in the dynamical plane.

Each render window has a set of controls that are unique to that window:
1. **Viewspace**: Controls the area of consideration for calculating and
rendering the fractal for the window. The subsections are:
    1. **Center**: The hypercomplex point that is the center of the viewspace.
    2. **View Axis Mapping**: Each dropdown corresponds to an axis in the rendering
    screen: x, y, and, for 3D mode, z. The values in each dropdown correspond to an
    element of the active number system. The selected value for each axis will map
    the number system element to the rendering axis. For example, rendering a 2D
    fractal using the complex number system, setting x to e₀ and y to e₁ will render
    the fractal using the standard of the horizontal axis representing the real part,
    and the vertical axis representing the imaginary part.  
    Beside each dropdown is a checkbox that allows the user to "invert" (or flip) the
    given axis.
2. **Iterations** (only for the *escape* render mode): There are 2 sliders in this section:
    1. **Iteration modifier**: This changes the maximum number of iterations to consider for
    a particular zoom level.[^6]
    2. **Iterations Per Frame**: The maximum number of iterations to calculate per rendering
    frame. A lower setting here will cause the fractal to render indistintly at first,
    gaining clearer definition per frame. This is especially noticable at larger zoom levels.
    A higher number can result in freezes and, on Windows, a forced invalidation of the
    OpenCL CommandQueue, crashing the program.

### Navigating in 2D rendring mode
Clicking the right mouse button (or dragging with it pressed) will update the current
*parameter* used in the dynamical views to be the point under the cursor.

Dragging the mouse with the left mouse button down will move the viewspace.

Using the scrollwheel (or two-finger scrolling for touchpads, if enabled in the OS) controls
the zoom level. Scrolling up will zoom in, and scrolling down will zoom out.

### Navigating in 3D rendring mode
There is currently no method to control the rendering viewspace or the *parameter* from
the 3D rendering mode (this is, however, on the roadmap). The rendering viewspace is
reused between 2D and 3D mode, so it is possible to update the view in 2D mode and switch
back to 3D. Caution must be used here, since the 3D mode uses ***much*** more VRAM than 2D.

Dragging the mouse with the left mouse button down will rotate the camera around the
viewspace using a trackball-like interface.

Using the scrollwheel will move the point of view to be closer or farther from the center of
the viewspace. Srolling up will move it closer, and scrolling down will move it farther out.

## Building
This program uses CMake as the build system, and requires the following open-source libraries:
* wxWidgets 3.2.6
* GLEW 2.1.0
* glm 1.0.1
* OpenCL 2.0

All of these libraries are avalible using vcpkg and CMake's
[`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html) function.

## Roadmap
### Near-term features
- [ ] Implment changing the rendering viewspace in 3D mode
- [ ] Implment updating the parameter from 3D mode
- [ ] Better error handling (rollback invalid changes?)
- [ ] Decouple fractal computation and rendering from main UI thread
- [ ] Implement zoom slider in viewspace control panel

### Future features
- [ ] Allow collapsing the control panels in the view windows
- [ ] Implement more number systems (split-complex, dual numbers, etc.)
- [ ] Implment more render modes (orbit traps, etc.)

## Contributing
If you want to contribute to this project, raise an issue and/or submit a pull request.
If you would like to financially support the project, you can do so using
[PayPal](https://paypal.me/waterglow).

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Miscellaneous development notes
The OpenCL kernels are all built at runtime using C preprocessor macro metaprogramming.
This is because my development machine is quite outdated, and does not support OpenCL C++.

For a detailed explanation of the ray-marching algorithm used in this program, see the
[RayMarching.md](RayMarching.md) file.

[^1]: Note that the resolution is ***not*** related to the size of the render window.
It is the size of the rendering textures and computation buffers. Currently, if the
requested render size exceeds the capacity of VRAM, the program will display an error
and exit. Better handling is on the roadmap.

[^2]: Phase-space is essentially a map of all possible dynamical-space configurations.
Each point 𝙋 in phase-space directly maps to the center point (0 + 0𝑖) of the
dynamical-space calculation with parameter 𝙋.

[^3]: Fractional escape is a classic fractal rendering technique that maps a color
value to a measure of how "quickly" that particular point escapes to infinity.

[^4]: The color value is determined by the position of the pixel/voxel in the rendering
texture. Red corresponds to the x-coordinate, blue to the y-coordinate, and (in 3D mode)
green corresponds to the z-coordinate.

[^5]: Dynamical-space is a representation of the behavior of a mathematical function
at each point with a given parameter 𝙋. 

[^6]: If the same maximum number of iterations is used for all zoom levels, the fractal
will have a much lower level of detail when zoomed. More iterations are required at greater
zoom levels in order to avoid this problem.
