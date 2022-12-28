#version 330 core

layout(location = 4) in vec3 inPos;	
layout(location = 5) in vec3 inPos2;	
layout(location = 6) in vec3 inPos3;	

layout(location = 7) in vec3 inColor;
layout(location = 8) in vec3 inColor2;
layout(location = 9) in vec3 inColor3;

layout(location = 10) in vec3 inNormal;
layout(location = 11) in vec3 inNormal2;
layout(location = 12) in vec3 inNormal3;

uniform mat4 modelViewProj;
uniform mat4 modelView;
uniform mat4 normal;

uniform mat4 modelViewProj2;
uniform mat4 modelView2;
uniform mat4 normal2;

uniform mat4 modelViewProj3;
uniform mat4 modelView3;
uniform mat4 normal3;

out vec3 vcolor1;
out vec3 vcolor2;
out vec3 vcolor3;

out vec3 vpos1;
out vec3 vpos2;
out vec3 vpos3;

out vec3 vnormal1;
out vec3 vnormal2;
out vec3 vnormal3;

void main()
{
	vcolor1 = inColor;
	vnormal1 = normalize((normal * vec4(inNormal, 0.0)).xyz);
	vpos1 = (modelView * vec4(inPos, 1.0)).xyz;

	vcolor2 = inColor2;
	vnormal2 = normalize((normal * vec4(inNormal2, 0.0)).xyz);
	vpos2 = (modelView2 * vec4(inPos2, 1.0)).xyz;

	vcolor3 = inColor3;
	vnormal3 = normalize((normal * vec4(inNormal3, 0.0)).xyz);
	vpos3 = (modelView3 * vec4(inPos3, 1.0)).xyz;
	
	gl_Position1 =  modelViewProj * vec4 (inPos,1.0);
	gl_Position2 =  modelViewProj2 * vec4 (inPos2,1.0);
	gl_Position3 =  modelViewProj3 * vec4 (inPos3,1.0);
}
