#version 430

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

uniform sampler2D theTexture;

void main()
{
    colour = texture(theTexture, TexCoord);
}