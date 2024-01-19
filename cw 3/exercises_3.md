## Perspective projection 

Exercises **main_3_1** starts where exercise **main_2_1** from previous class should end. You can see only a square not the whole box. We will display it with perspective projection, what will give us ilusion of depth.

#### Exercise 1

Perspective matrix is created by a function `createPerspectiveMatrix()`, look into definition of this function. Send a result of the function to shader.

Result is not exactly what we would expect. Reason for this is now the camera is inside the box. Add camera matrix to the transformation (this will allow us to move it with keyboard keys) and translate the box in such a way that the camera would land outside of it.

#### Exercise 1*

Modify the perspective matrix. Inside the function `createPerspectiveMatrix()` is a variable `frustumScale`. It is responsible for field of view, modify it’s value and see what effect is produced. To calculat is use a formula $ S = \frac{1}{tan\left( \frac{fov}{2}* \frac{\pi}{2}\right)} $​, where $fov$ is the viewing angle.

#### Exercise 1**

Still we have a problem with stretching when the window shape is changed. Solution is to have different values of **frustumScale** for **X** and **Y** coordinates. For this use a function `onReshape`,which is called every time the screen size is changed. First calculate screen ratio and assign the result to a global variable `screenRatio`. Next multiply appropriate coordinate by `screenRatio` in  `createPerspectiveMatrix()`.

## Shaders 

In this part we will use different objects than the box. Remove it’s initialization form `init` function and it’s drawcall in `renderScene`. Replace shaders to `shaders/shader_3_2.vert` and `shaders/shader_3_2.frag`. 

#### Exercise 2 

Load a spaceship and a sphere with `loadModelToContext` to according variables `Core::RenderContext`. Next display them with `Core::DrawContext` function. Finally place them in diferent locations in space.

##### Add possibility to change colours of different objects:

Create a variable  `uniform vec3 objectColor` in the file  `shader_3_2.frag`, which you will use to send the colour of an object in RGB format to the shader.
 The built-in shader variable `gl_FragColor` has type vec4 - RGBA. Set the .rgb part of the variable equal to `objectColor`, and the 4th element (.a) - `1.0f`.
 Now you can control the color of the drawn object, by sending values to the variable `objectColor` using the `function glUniform3f( int location, float v0, float v1, float v2 )`.

 "location" indicates the address of the variable in GPU memory we want to send data to.
 You can retrieve this address with the function `glGetUniformLocation(program, "variable name")`.

#### Exercise 2*

Modify program in such a way, that colour of the ship would change in time.

#### Exercise 3

 We want to display the normal vectors at each fragment as colors.

 The normal vector of a vertex is accessible in the vertex shader (file with the extension **.vert**). The pixel color is defined in the fragment shader (**.frag**).
 Hence, we have to send the normal vector from vertex to fragment shader. The normal vector will be automatically interpolated.

 Create a variable `out vec3 name` in the vertex shader and a corresponding variable `in vec3 name` in the fragment shader.
 Set the values in the vertex shader for the new variable - this will automatically set the value in the fragment shader. Keep in mind that you can only set variable values in the `main()` part of a shader.
 Use the new variable in the fragment shader to set the color of pixels: normalize the new variable and scale all the values to the range [0, 1] and use the result to define the R, G, B channels of the variable `gl_FragColor`.

#### Exercise 4

Create a function `drawObject` in the .cpp source file, which will make drawing objects more convienient.
 As input parameters we need a reference to `Core::RenderContext` as well as the model matrix of the object.
 Display a few objects in different positions (and look at the from different perspectives).

 "Attach" the spaceship to the camera - its model matrix has to take into account the position and orientation of the camera such that it gives the impression you are steering the spaceship. 

```C++
glm::mat4 shipModelMatrix = 
	glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0,-0.25f,0)) * 
	glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) *
    glm::scale(glm::vec3(0.25f));
```

Analize the above instruction and evaluate how else to perform the same task.

#### Exercise 5

Create a planetary system - with one immovable sphere at the center (sun) and a few orbiting planets with moons around it.

