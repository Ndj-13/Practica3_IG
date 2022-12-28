#version 330 core

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec2 inTexCoord2;
layout(location = 2) in vec2 inTexCoord3;

out vec2 texCoord1;
out vec2 texCoord2;
out vec2 texCoord3;

void main()
{
	texCoord1 = inTexCoord;

	texCoord2 = inTexCoord2;

	texCoord3 = inTexCoord3;

}
