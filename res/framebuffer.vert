// Version of GLSL we are using
#version 450 core

// Layout help OpenGL read the vertex data it receives

// At the 0th location of the layout their is a vec4 aPos
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec2 aTex;
// Outputs a vec3 named texCoord
out vec2 texCoord;

void main()
{
   // Assigns gl_Position 
   gl_Position = aPos;
   texCoord = aTex;
}