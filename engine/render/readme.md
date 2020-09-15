Mesh Resources
Your assignment is to design a class that represents a mesh. You should also use programmable shaders to colorize and move + rotate a mesh object.

Create a new MeshResource class that will be used later on to  represent a mesh when rendering.
It has to contain at least a vertex buffer, index buffer and vertex attributes/layout/structure.
Add support for binding the contained buffers and attributes.
User the MeshResource designed to:
Create a quadrilateral.
Render it to screen with different colors for each vertex using programmable shaders.
Modify the transform
Rotate the quad from earlier on the screen using your math library.
Rotation is to be continuous (the quad rotates slowly).
Add continuous movement to the quad on the screen from left to right and back.
Place all files that is related to math or rendering neatly in the engine sub-folders in your repository.

Your test program for this assignment should be placed within a separate project in the project sub-folder (see previous assignment on how to do this).

Tip: Try to make the resource as decoupled from OpenGL as possible so that if you were to change or add another rendering API you wouldn't need to rewrite the entire class.

 

Delivery
Use the same repository as you used earlier.
Present your completed assignment to Fredrik
Place your completed assignment in the corresponding folder in your repository, commit, push and then upload the url of your repository to the Canvas submission. Make sure you don't commit and Visual Studio specific files (ex: .sln, .vcxproj), binaries (ex: .exe, .pdb) or temporary files.
NOTE: Remember to comment your code thoroughly. Make sure you build, run and test your submission before handing it in.