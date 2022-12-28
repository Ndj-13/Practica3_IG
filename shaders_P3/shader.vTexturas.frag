#version 330 core

out vec4 outTexture;

in vec2 texCoord1;
in vec2 texCoord2;
in vec2 texCoord3;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
//debemos pedir identificador pa decirle en q puerto mirar cada textura

uniform sampler2D colorTex2;
uniform sampler2D emiTex2;

uniform sampler2D colorTex3;
uniform sampler2D emiTex3;

//Propiedades del objeto
vec3 Ka1;
vec3 Ka2;
vec3 Ka3;

//kd = ka

vec3 Ks;

vec3 Ke;
vec3 Ke2;
vec3 Ke3;



void main()
{
	outTexture = texture (colorTex, texCoord1);
	outTexture += texture (colorTex2, texCoord2);
	outTexture += texture (colorTex3, texCoord3);
  
}
