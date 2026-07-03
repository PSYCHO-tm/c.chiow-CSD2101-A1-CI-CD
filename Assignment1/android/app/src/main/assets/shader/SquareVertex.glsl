#version 300 es
layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec3 VertexColor;

out vec4 SquareColor;

void main()
{
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
    SquareColor = vec4(VertexColor, 1.0);
}
