#version 420 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 inputColor;

layout (location = 1) out vec3 colorOutput;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);

    colorOutput.x = inputColor.x;
    colorOutput.y = inputColor.y;
    colorOutput.z = inputColor.z;
}