#version 420 core

layout (location = 1) in vec3 color;
out vec4 FragColor;

void main()
{
   FragColor = vec4(color.x, color.y, color.z, 1.0f);
} 