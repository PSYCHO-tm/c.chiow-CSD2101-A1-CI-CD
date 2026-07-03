#version 300 es
precision mediump float;

in  vec4 SquareColor;
out vec4 FragColor;

void main()
{
    FragColor = SquareColor;
}
