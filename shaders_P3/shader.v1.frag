#version 330 core

out vec4 outColor;

in vec3 vcolor;
in vec3 vpos;
in vec3 vnormal;
in vec2 texCoord;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
//debemos pedir identificador pa decirle en q puerto mirar cada textura

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
float alpha = cos(radians(5000.0));
vec3 Ke;

//Propiedades de la luz
vec3 Ia = vec3 (0.1);
vec3 Id = vec3 (0.2);
vec3 Is = vec3 (0.2);
vec3 lpos = vec3 (0.0); 
vec3 pp;
float n;

vec3 shade();

void main()
{
	Ka = texture(colorTex, texCoord).rgb;
	Kd = Ka;
	Ke = texture(emiTex, texCoord).rgb;
	Ks = vec3 (1.0);

	N = normalize (vnormal);
	pp = vpos;

	n = 50;
	
	outColor = vec4(shade(), 1.0);   
}

vec3 shade()
{
	vec3 c = vec3(0.0);
	c += Ia * Ka;

	vec3 L = normalize (lpos - pp);
	vec3 diffuse = Id * Kd * dot (L,N);
	//c += clamp(diffuse, 0.0, 1.0);
	c += Id * Kd * dot(N, L);

	vec3 V = normalize (-pp);
	vec3 R = normalize (reflect (-L,N));
	//vec3 specular = Is*Ks*pow(factor,alpha);
	c += Is * Ks * pow(max(dot(R, V), 0), n);

	//c += clamp(specular, 0.0, 1.0);

	c+=Ke;

	c = clamp(c, 0, 1);
	
	return c;
}
