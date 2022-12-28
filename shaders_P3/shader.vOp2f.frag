#version 330 core

out vec4 outColor;

in vec3 vcolor;
in vec3 vpos;
in vec3 vnormal;
in vec2 texCoord;

in vec3 vTangent;
in vec3 vbiTangent;

uniform sampler2D colorTex;
uniform sampler2D emiTex;
uniform sampler2D normalTex;

uniform mat4 viewL;
uniform vec3 posicionL;
uniform vec3 intensidadL;

//Propiedades del objeto
vec3 Ka;
vec3 Kd;
vec3 Ks;
vec3 N;
vec3 Ke;


//Propiedades de la luz
vec3 Ia = vec3 (0.3);
//le damos el valor de intensidad de luz a Id e Is:
vec3 Id = intensidadL.xyz;
vec3 Is = intensidadL.xyz;
vec3 lpos = vec3 (viewL * vec4(posicionL, 1.0)).xyz;

//El segundo foco no lo controlamos por teclado:
vec3 Id2 = vec3(15.0);
vec3 Is2 = vec3(15.0);
vec3 lpos2 = vec3(0.0, 0.0, 0.0); //segundo foco (luz focal)

vec3 lpos3 = vec3(5.0, 4.0, -10.0);

vec3 D = vec3 (0.0, 0.0, -1.0);
float alpha = cos(radians(20.0));
float e = 3.0;
float fdir = 0.0;

vec3 pp;

float n;

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

	pp = vpos;

	mat3 bumpMap = mat3 (vTangent, vbiTangent, vnormal);
	N = 2*(texture(normalTex, texCoord).rgb) - vec3 (1.0); // 2*N -1
	N = normalize (bumpMap * N);

	n = 200;
	
	outColor = vec4(shade(), 1.0);   
}

vec3 shade()
{
	vec3 c = vec3(0.0);
	
	//Atenuacion
	float d = length(pp);
	float f = 1.0 - pow((d/dmax), 4.0);
	if (f < 0)
	{
		f = 0;
	}
	fwin = pow (f, 2.0);
	fdist = (pow((d0/(max (d, d0))), 2.0)) * fwin;

	//Luz focal
	float dotFocal = dot (normalize (pp-lpos2), normalize(D));
	if (alpha < dotFocal)
	{
		fdir = pow ((dotFocal - alpha)/(1.0 - alpha), e);
	}

	c = Ia * Ka;

	//difusa
	vec3 L = normalize (lpos - pp);
	c += Id * fdist * Kd * dot(N, L);

	//difusa segundo foco
	vec3 L2 = normalize (lpos2 - pp);
	c += Id2 * fdist * fdir * Kd * dot(N, L2);

	//difusa tercer foco (direccional)
	vec3 L3 = normalize (-D);
	c += Id2 * fdist * fdir * Kd * dot(N, L3);

	//Especular
	vec3 V = normalize (-pp);
	vec3 R = normalize (reflect (-L,N));
	c += Is * fdist * Ks * pow(max(dot(R, V), 0), n);

	//especular segundo foco
	vec3 R2 = normalize (reflect (-L2, N));
	c += Is2 * fdist * fdir *Ks * pow(max(dot(R2, V), 0), n);

	//especular tercer foco
	vec3 R3 = normalize (reflect (-L3, N));
	c += Is2 * fdist * fdir *Ks * pow(max(dot(R3, V), 0), n);

	c += Ke;

	c = clamp(c, 0, 1);
	
	return c;
}
