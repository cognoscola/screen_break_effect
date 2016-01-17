# Screen Shatter Effect
As seen on final fantasy X

Check out the final result here (https://www.instagram.com/p/BASypuDNgRS/)

Install:
- copy the entire project to any directory
- Go to install directory in a terminal
- type without quotes: "cmake ." to generate a makefile
- type without quotes: "make" to generate an executable
- run executable.

Controls
- a,s,d,w  - move camera
- mouse    - orient camera
- b        - take a snapshop (will freeze for a few seconds)
- spacebar - shatter the screen
- p        - record 10 seconds worth of frames

If you manage to get the project running on your computer, pressing and Holding the SPACEBAR will make the screen
shatter.

Final Fantasy X has a transition effect whenever you enter a battle in the game. The screen shatters
and the pieces fly away, revealing the battle screen. Heres how I did it. This is by no means the best way to do it.
There are better more efficient ways.

First I created a quad to fit directly in front of the camera. Except I did not create this quad with 2 triangles,
but with many triangles. In order to create many triangles I used a Delaunay Triangle Algorithm
(http://www.geom.uiuc.edu/~samuelp/del_project.html). A very well put together working algorithm of this sort can be
found here (https://github.com/eloraiby/delaunay). The algorithm takes any number of points and spits out indices that
mark the corners of each triangle. We can generate a big quad made out of triangles by placing many random points inside
our quad. I also put four additional points on the corners of our quad and a few additional points along the
edge of the quad so that we don't end up with very large triangles that span the entire width or height of the
quad. More random points means more triangles.Once we have our points we create our vertex array object and two vertex buffer objects, one which holds triangle
coordinates and the other to hold texture coordinates. Getting texture coordinates is just a matter of converting
the triangle points from a local space coordinate to a UV coordinate space.

Next I animated every triangle. When we look at the effect in the game ( https://www.youtube.com/watch?v=nXGlKZzCCtE)
we see that there are two stages to the animation. In the first stage, the screen glass shatters in place and every
piece seems to have transformed by a small amount. In the 2nd stage, the pieces exit the screen on the right side
starting with the right most pieced and propagating the order to the left. To simulate the first stage, I simply
rotate the pieces around the Z axis (out of the screen) by a random value between -5 and 5 degrees. This animation
is no more then half of a second. Next, I translate the pieces to the right while simultaneously giving them a random
translation in the Y axis and a random rotation around the X axis. For more information about animating objects
see the code or see my other project ( https://github.com/cognoscola/hardware_skinning).

In the game, you'll notice that the screen flashes white when the shatter happens, followed by a fade into the glass,
and then there is a black screen behind the glass that fades out to show the next screen. This was very simple to
replicate. I disabled depth test, enabled blending, then proceeded to first draw a black quad, followed by the glass,
followed by a white quad. Then i faded out each quad in a timely manner similar to that shown in the game.

The last thing to do was to texture the glass. I created a framebuffer texture object and captured only one frame
and slapped that on the the glass quad.

