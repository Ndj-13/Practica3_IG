#version 330 core

out vec4 outColor;

in vec3 vcolor;
in vec3 vpos;
in vec3 vnormal;
in vec2 texCoord;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
//debemos pedir identificador pa decirle en q puerto mirar cada textura

uniform mat4 viewL;
uniform vec3 posicionL;
uniform vec3 intensidadL;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
float alpha = cos(radians(5000.0));
vec3 Ke;

float e = 3.0;
float dir = 0.0;

//Propiedades de la luz
vec3 Ia = vec3 (0.3);
//le damos el valor de intensidad de luz a Id e Is:
vec3 Id = intensidadL.xyz;
vec3 Is = intensidadL.xyz;
vec3 lpos = vec3 (viewL * vec4(posicionL, 1.0)).xyz; 
//vpos = (modelView * vec4(inPos, 1.0)).xyz;
//hacemos como en el shader de vertices para calcular pos pero esta vez para la luz

vec3 D = vec3 (0.0, 0.0, -1.0);

vec3 pp;
float n;
float fdir = 0.0;

//Atenuacion
float fdist = 0;
float d0 = 1.0;
//dmin = d0 = 1
float fwin = 0;
float dmax = 50.0;

vec3 shade();

void main()
{
	Ka = texture(colorTex, texCoord).rgb;
	Kd = texture(colorTex, texCoord).rgb;
	Ke = texture(emiTex, texCoord).rgb;
	Ks = vec3 (1.0);

	N = vnormal;
	pp = vpos;

	n = 50;
	
	outColor = vec4(shade(), 1.0);   
}

vec3 shade()
{
	vec3 c = vec3(0.0);

	c = Ia * Ka;

	vec3 L = normalize (lpos - pp);
	//vec3 diffuse = Id * Kd * dot (L,N);
	//c += clamp(diffuse, 0.0, 1.0);
	c += Id * Kd * dot(L, N);

	vec3 V = normalize (-pp);
	vec3 R = normalize (reflect (-L,N));
	//float factor = max (dot (R,V), 0.01);
	//vec3 specular = Is*Ks*pow(factor,alpha);
	c += Is * Ks * pow(max(dot(R, V), 0), n);

	//c += clamp(specular, 0.0, 1.0);

	c += Ke;

	c = clamp(c, 0, 1);
	
	return c;
}
