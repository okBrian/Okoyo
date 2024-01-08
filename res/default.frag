// Version of GLSL we are using
#version 450 core
// Outputs a vec4 color
layout(location  = 0) out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
   FragColor = texture2D(tex0, texCoord);  
}