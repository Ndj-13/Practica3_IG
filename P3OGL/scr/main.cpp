#include "BOX.h"
#include "auxiliar.h"


#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> //tiene muchos eventos q podemos usar

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>



//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices --> variables globales para acceder a ellas desde cualquier sitio --> esto escala mal en renderizado potente
glm::mat4 proj = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 model2 = glm::mat4(1.0f); //para segundo cubo
glm::mat4 model3 = glm::mat4(1.0f); //tercer cubo

//variables en memoria del cliente (la tarjeta grafica no tiene constancia de q existe) --> no tiene impacto sobre renderizado a no ser q las subamos
 

//Propiedades de intensidad y posicion de la luz
glm::vec3 intensidadL = glm::vec3(0.5f);
glm::vec3 posicionL = glm::vec3(0.0, 5.0, 0.0);


//Variables de poscion
glm::vec4 cop = glm::vec4(0.0f, 0.0f, 11.0f, 1.0f);
glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
glm::vec4 lookAt = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
glm::vec4 right = glm::vec4(glm::cross(glm::vec3(lookAt), glm::vec3(up)), 0.0f);


//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////
//Por definir

//Un programa para las texturas
//1ºpaso es dar identificador a elementos para poder modificarlos a traves de funciones
unsigned int vshader; //identificador del objeto shader de vertices
unsigned int fshader; //identificador del shader de fragmentos
unsigned int program; //identificador de ambos shaders juntos --> los enlazamos en un programa
//shaders q trabajan juntos se meten en un programa


//Atributos --> identificador por cada variable uniform q tenemos q subir (en vez de puntero) --> para subir info al vertice, donde subirla
//info asociada a vertices --> no podemos subir info cada vez q renderizamos--> la subimos antes

//variables uniformes:
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;
int uViewL;

int uIntensidadL;
int uPosicionL;

//Texturas Uniform --> identificadores 
int uColorTex;
int uEmiTex;
int uNormalTex;
int uSpecTex;
//les damos valor despues de hacer el enlazado (link)

//Atributos
int inPos;
int inColor;
int inNormal;
int inTexCoord; //para el programa de texturas
int inTangent;

//mas identificadores
//VAO 
unsigned int vao;
//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int tangentVBO;
unsigned int atrVBO;
unsigned int triangleIndexVBO; //vbo de almacenar indices de como recorrer atributos para crear primitivas (explicado en apuntes)

//unsigned int atrVBO2;
//unsigned int triangleIndexVBO2;

//Texturas --> para el programa de texturas
unsigned int colorTexId;
unsigned int emiTexId;
unsigned int normalTexId;
unsigned int specTexId;

//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////
//!!Por implementar

//Declaración de CB --> call back: tratan eventos --> funcion a la q se llama cuando se produce un det evento
void renderFunc(); //la llamamos cada vez q queramos renderizar
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

//Funciones de inicialización y destrucción (del contexto y extensiones)
void initContext(int argc, char** argv);
void initOGL();
void initShader(const char *vname, const char *fname);
void initObj();
void destroy(); //cuando terminamos destruimos objetos


//Carga el shader indicado, devuele el ID del shader
//!Por implementar
GLuint loadShader(const char* fileName, GLenum type); //podemos cargar los 2 en la misma funcion devolviendo el id del shader

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
//!!Por implementar
unsigned int loadTex(const char *fileName);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();
	initShader("../shaders_P3/shader.vOp2g.vert", "../shaders_P3/shader.vOp2g.frag");
	initObj();

	glutMainLoop(); //tras terminar inicializacion y antes de limpiar creo

	destroy();

	return 0;
}
	
//////////////////////////////////////////
// Funciones auxiliares 
void initContext(int argc, char** argv)  //Inicializar contexto con glut
{
	glutInit(&argc, argv); //inicializar context
	glutInitContextVersion(3, 3); //opengl define estandar --> version de open gl q vamos a usar (3.3)
	glutInitContextProfile(GLUT_CORE_PROFILE); //creamos contexto compatible --> 
	//aun no podemos hacer llamada a opengl --> contexto no se ha creado ( le estamos diciendo q tipo queremos pero no lo hemos creado
	//profile --> version nueva de opengl
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //quiero q sea doble buffer, q sea rgba y profundidad --> define frame buffer por defecto
	//glutInitWindowSize(500, 500); //funcion q crea la ventana --> momebnto de creacion efectiva del contexto --> ahora ya le podemos empezar a llamar
	//glutInitWindowPosition(0, 0); //0,0 es esquina superior izq en sist operativo --> en opengl es justo el de abajo
	glutCreateWindow("Prácticas OGL");
	//frame buffer: conjunto de bufferes donde almacenamos info de pixeles --> salida del cauce
	//Default frame buffer: frame buffer q usamos patrta mostrar resultado por pantalla 
		//(el resto se quedan almacenados pero no se muestran, solo se muestra el default)
		//compuesto por zbuffer, stecilbuffer, buffers de color (muestro uno, pinto otro), 
		//pixel ownership buffer (saber si hay ventana q esta tapando parte de ventana de renderizado)

	GLenum err = glewInit(); //inicializar extensiones (una vez creado contexto)
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}
	const GLubyte* oglVersion = glGetString(GL_VERSION); //pido q devuelva version de contexto --> chquear q lo he creado bn
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	//le decimos al glut q llame a estas funciones para tratar eventos:
	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc); 

}

void initOGL() //configuracion inicial del cauce
{
	glEnable(GL_DEPTH_TEST); //test de profundidad --> lo activamos (esta desacivado)
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f); //antes de pintar hay q limpiar --> limpiamos con un color para borrar lo q habia abajo
		//escogemos color gris
	glFrontFace(GL_CCW); //CCW: sentido antihorario--> defino cuando pase poligonos q van a estar ordenados en sentido antihorario
	//asi etapa de culling sabe si esta mirando a la camara
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //posibilidad de renderizar en modo fill (fragmentos intermedios)
	 //si ponemos GL_LINES pinta lineas (creo q esta en el powerpoint)
	glEnable(GL_CULL_FACE); //habilitamos, activamos el culling (clipping es automatico pero culling hay q activarlo)
	
	proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 50.0f);
	view = glm::mat4(1.0f);
	view[3].z = -6;
	//Variables en memoria prinicpal en CPU --> para usarlas las subimos a la tarjeta grafica
}

void destroy_()
{
	glDetachShader(program, vshader); //al programa le quita el shader de vertices
	glDetachShader(program, fshader); //al programa le quita shader de fragmentos
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program); //borramos el programa

	/*
	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);
	glDeleteVertexArrays(1, &vao);
	*/
	//VAO (configuracion de la geometria) independiente del shader --> por eso no tiebe sentido lo del -1, lo destruimos siempre
	glDeleteBuffers(1, &posVBO);
	glDeleteBuffers(1, &colorVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	glDeleteVertexArrays(1, &vao);
}

void destroy()
{
	glDetachShader(program, vshader); //al programa le quita el shader de vertices
	glDetachShader(program, fshader); //al programa le quita shader de fragmentos
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program); //borramos el programa

	glDetachShader(program, vshader); 
	glDetachShader(program, fshader); 
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program); 

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);
	glDeleteTextures(1, &normalTexId);
	glDeleteTextures(1, &specTexId);

	glDeleteBuffers(1, &atrVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	glDeleteVertexArrays(1, &vao);
}

void initShader(const char *vname, const char *fname)
{
	vshader = loadShader(vname, GL_VERTEX_SHADER); //le decimos cual es el codigo de shader de vertices y q lo cree
	//recibe cadena de texto q tiene nombre donde esta el shader y el shader (creo)
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);

	program = glCreateProgram(); //constructor: creacion y q devuelva manejador
	glAttachShader(program, vshader); //a este programa asigno shader de vertices q he creado ahi arriba
	glAttachShader(program, fshader);
	glLinkProgram(program); //los enlazamos pa q trabajen juntos

	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked); //los compilamos y lincamos en un programa --> nos devuelve enlace pa ese programa (link)
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(program, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;
		glDeleteProgram(program);
		program = 0;
		exit(-1);
	}

	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");
	uViewL = glGetUniformLocation(program, "viewL");
	//para cada variable devuelve identificador para q les pueda asignar valor
	
	uPosicionL = glGetUniformLocation(program, "posicionL");
	uIntensidadL = glGetUniformLocation(program, "intensidadL");

	//Pedimos q devuelva identificadores atributos -> devuelve -1 si no existe variable o no se usa (ejemplo con lo de pepe)
	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");
	inTangent = glGetAttribLocation(program, "inTangent");

	//inicializamos valores texturas--> asi shader sabe q puerto tiene q mirar
	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");
	uNormalTex = glGetUniformLocation(program, "normalTex");
	uSpecTex = glGetUniformLocation(program, "specTex");
	
	//std::cout << glGetUniformLocation(program, "Pepe") << std::endl; 

	glUseProgram(program); //antes de renderizar le decimos de q puerto (texture unit) tienen q coger la textura
	//asi cuando quiera renderizar con nueva textura solo tengo q cambiar texture unit y activarla ahi
	//activamos objetos cuando lo vamos a renderizar o cuando lo voy a configurar

	
	//vamos a ver si uColorTex y uEmiTex son distintas de 1 para saber si shader la usa
	if (uColorTex != -1) glUniform1i(uColorTex, 12);
	//la i es de int (de glUniformi)
	//variable a la q voy a subir info --> uColorTex: puerto donde buscar la textura
	if (uEmiTex != -1) glUniform1i(uEmiTex, 13);

	if (uNormalTex != -1) glUniform1i(uNormalTex, 14);

	if (uSpecTex != -1) glUniform1i(uSpecTex, 15);
	
}


void initObj_()
{
	//Crear 5 objetos buffers:
	//glGenBuffers(1, &posVBO); //pedimos identificador para el buffer donde almaeno posiciones
	//glGenBuffers(1, &colorVBO);
	//glGenBuffers(1, &normalVBO);
	//glGenBuffers(1, &texCoordVBO);
	//glGenBuffers(1, &triangleIndexVBO);
	//esto lo podemos hacer de forma mas eficiente --> hacer todas las peticiones a la vez
	//creamos buffer de unsigned:

	unsigned int buff[5]; //espacio para 5 identificadores
	glGenBuffers(5, buff);
	posVBO = buff[0];
	colorVBO = buff[1];
	normalVBO = buff[2];
	texCoordVBO = buff[3];
	triangleIndexVBO = buff[4];
	//de esta forma hacemos una sola llamada

	//ahora los vamos activando y les vamos reservando espacio:

	glBindBuffer(GL_ARRAY_BUFFER, posVBO); //se encarga de activar el VBO como array buffer

	//Configurar: reservar tamaño q vamos a necesitar para ese buffer (reserva espacio en tarjeta grafica) y sube datos:
	//glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
		//cubeVertexPos, GL_STATIC_DRAW);
	//tipo de buffer al q vamos a subir datos: ARRAY_BUFFER --> buffer de propiedades de vertices (atributos)
	//espacio q tiene q reservar: numero de vertices q tengo --> lo pide en bites (sizeof devuelve lo q ocupa variable float (32 bits o 4 bites), 
	//cada vertice tiene 3 coordenadas
	//cubeVertexPos: array donde se almacenan posiciones
	//STATIC_DRAW: es q lo voy a usar para dibujar y no lo voy a modificar --> uso q voy a hacer de la funcion
	//draw lo usamos como entrada del buffer

	//Ahora vamos a hacer lo mismo en 2 pasos: primero creamos espacio con BufferData (null) y luego subimos info con BufferSubData

	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
		NULL, GL_STATIC_DRAW);
	//deja espacio vacio
	//null --> reserva espacio --> no sube datos


	//buffersubdata solamente sube datos --> no reserva espacio --> actualizar valores de las posiciones
	glBufferSubData(GL_ARRAY_BUFFER, 0, cubeNVertex * sizeof(float) * 3,
		cubeVertexPos);
	//opset: primera posicion sobre la q voy a escribir (0)
	//tamaño: cubeNVertex....
	

	//Color
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO); //activar VBO donde voy a almacenar el color
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3, //3 coords tambn: r, g, b
		cubeVertexColor, GL_STATIC_DRAW);

	//Normal
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
		cubeVertexNormal, GL_STATIC_DRAW);

	//Textura
	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2, //vector de 2 componentes
		cubeVertexTexCoord, GL_STATIC_DRAW);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);
	//element array buffer = vbo de indices
	//element array buffer: subo la info al ultimo q active//numero de triangulos en vez de de vertices
	//subo el array donde almaceno indices de mi geometria

	//en el vao configuro como leer estos vbo y con q atributos enlazarlo

	//openGl sabe donde reservar porq coge ultimo arrayBufer q he activado --> si tenemos una variable activa y activamos otra se desactiva la anterior

	//unsigned int vaoArray[3]; //variable array q tiene 3 ints --> puedo recibir 3 vaos
	//Lo q hariamos ahora seria
	//glGenVertexArrays(3, vaoArray); //creame 3 identificadores y me los pones en este array vao
	//No reserva memoria  porq no se encarga de destruirla --> lo tiene q hacer el q se encarga de destruirla
	//yo soy el q reservo espacio y open gl me da 3 valores
	//si metiesemos un 1: (1, vaoArray) --> solo devolveria 1 objeto (rellena 1 casilla de las 3 solo)
	//&f --> meto variable en la posicion en la q apunta el puntero --> esto es de c++: repasarlo

	//creo vao y configuro vbo:
	glGenVertexArrays(1, &vao); //1: numero de objetos q queremos crear y le pasamos array para q nos meta los objetos o identificador (no me he enterado)
	glBindVertexArray(vao); //activa para poder configurarlo
	//estructura siempre la misma: crearlo y asignalre identificador y activarlo

	//Solo hay un objeto activo de un tipo a la vez

	glBindBuffer(GL_ARRAY_BUFFER, posVBO);//configuro este vbo en ese vao
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //uso esta funcion para configurar
	//no le digo ni q vbo voy a configurar ni el vao --> se almacena en vao activo y se configura el vbo activo --> lo coge del contexto
	//a inPos le hemos dado el identificador 0
	//coge el atributo 0 de posvbo --> vector de 3 componentes de tipo float
	//flase = no lo normalices
	//straight y opset a 0, 0

	//Esto lo hacemos para todos nuestros atributos
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO); //lo configuramos dentro del vao activo
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//float --> dice q es tipo float
	//false --> q no lo normalize

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
	

	//Estas funciones activan uso del atributo en el caso de q haya algo q activar
	//Si es menos 1 deshabilito --> los habilitamos porq estan deshabilitados por defecto
	if (inPos != -1)
	{
		//glGenBuffers(1, &posVBO); --> la ponemos arriba
		//glBindBuffer(GL_ARRAY_BUFFER, posVBO); //se encarga de activar el VBO como array buffer
		//glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			//cubeVertexPos, GL_STATIC_DRAW);
		//glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);

		//dejamos solo ultima linea --> el resto lo hemos hecho fuera
		//cuando queremos renderizar con varios shaders 
		//los atributos q no tiene definidos no los usa, por eso usa los q no son -1
	}
	if (inColor != -1)
	{
		//glGenBuffers(1, &colorVBO);
		//glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		//glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			//cubeVertexColor, GL_STATIC_DRAW);
		//glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}
	if (inNormal != -1)
	{
		//glGenBuffers(1, &normalVBO);
		//glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		//glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3,
			//cubeVertexNormal, GL_STATIC_DRAW);
		//glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}
	if (inTexCoord != -1)
	{
		//glGenBuffers(1, &texCoordVBO);
		//glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		//glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 2,
			//cubeVertexTexCoord, GL_STATIC_DRAW);
		//glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}

		//glGenBuffers(1, &triangleIndexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);  //cada vez q vaya a renderizar saca los elementos de aqui
		//lo bindeamos 2 veces: uno pa subir info (arriba) y otra pa q vao sepa q este es el buffer de indices
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			//cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
			//GL_STATIC_DRAW);

		model = glm::mat4(1.0f); //incializamos aqui la variable porq lo ha decidio el profe

		//colorTexId = loadTex("../img/color2.png");
		//emiTexId = loadTex("../img/emissive.png");
}

void initObj()
{
	//Vamos a crear un macrobuffer --> atrVBO para todos los atributos
	unsigned int buff[2]; //espacio para 2 identificadores
	glGenBuffers(2, buff);

	atrVBO = buff[0];
	triangleIndexVBO = buff[1];
	
	glBindBuffer(GL_ARRAY_BUFFER, atrVBO); 

	glBufferData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 14, //3 para posiciones, 3 para color, 2 para textura y 3 para la normal
		NULL, GL_STATIC_DRAW);
	//reserva espacio para 11 posiciones

	glBufferSubData(GL_ARRAY_BUFFER, 0, cubeNVertex * sizeof(float) * 3,
		cubeVertexPos);
	glBufferSubData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * 3, /*empieza donde acaba el anterior*/
		cubeNVertex * sizeof(float) * 3, cubeVertexColor);
	glBufferSubData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * (3+3), /*dejamos espacio de pos y de color*/
		cubeNVertex * sizeof(float) * 3, cubeVertexNormal);
	glBufferSubData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * (3+3+3), /*espacio de pos, color y normal*/
		cubeNVertex * sizeof(float) * 2, cubeVertexTexCoord);
	glBufferSubData(GL_ARRAY_BUFFER, cubeNVertex * sizeof(float) * (3+3+3+2), /*espacio de pos, color, normal y tangente*/
		cubeNVertex * sizeof(float) * 3, cubeVertexTangent);
	//subimos uno a continuacion de otro
	// //subimos coords de textura
	//esto simplifica configuracion de vao

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex,
		GL_STATIC_DRAW);

	//creo vao y configuro vbo:
	glGenVertexArrays(1, &vao); 

	glBindVertexArray(vao); 

	glBindBuffer(GL_ARRAY_BUFFER, atrVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //el opset es el ultimo 0 e indica desde donde leer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (cubeNVertex * sizeof(float) * 3)); //aqui leemos desde donde se quedo la ultima posicion
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(cubeNVertex * sizeof(float) * (3+3)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)(cubeNVertex * sizeof(float) * (3+3+3)));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)(cubeNVertex * sizeof(float) * (3+3+3+2)));
	//le seguimos poniendo los identificadoores de cada atributo para q sepa si se trata de posicion, color...

	/*
	//Esto parte opcional para reordenar buffer
	//configurar coords de textura
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, 0); //consigo el siguiente
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * 3)); //primera posicion donde encontramos una posicion
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(sizeof(float) * (3 + 3)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)( sizeof(float) * (3 + 3 + 3)));
	//no he entendido por q se quita en cubeNVertex* --> preguntar
	//Ahora atributos contrapeados
	//Array buffer: vao --> donde vamos a configurar vbo creo
	*/

	if (inPos != -1)glEnableVertexAttribArray(inPos);

	if (inColor != -1)glEnableVertexAttribArray(inColor);
	
	if (inNormal != -1)glEnableVertexAttribArray(inNormal);

	if (inTexCoord != -1)glEnableVertexAttribArray(inTexCoord);

	if (inTangent != -1)glEnableVertexAttribArray(inTangent);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);  

	model = glm::mat4(1.0f); 
	model2 = glm::mat4(1.0f);
	model3 = glm::mat4(1.0f);

	emiTexId = loadTex("../img/emissive.png");
	colorTexId = loadTex("../img/color.png");
	normalTexId = loadTex("../img/normal.png");
	specTexId = loadTex("../img/specMap.png");

}

//GLuint loadShader(const char *fileName, GLenum type){ return 0; }
GLuint loadShader(const char* fileName, GLenum type) //funcion auxiliar q creamos 
//crear un shader (le paso fichero donde esta codigo e info adicional (q shader es(vertices, fragmentos...))
{
	unsigned int fileLen;
	char* source = loadStringFromFile(fileName, fileLen); //devuelve nombre y cadena de caracteres (codigo del fichero q hemos cargado)
	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader; //variable donde meto id del shader
	shader = glCreateShader(type); //funcion q uso pa crear objeto shader (typoe --> tipo de shader q quiero crear(vertices, geometrico...)) 
	//devuelve identificador
	glShaderSource(shader, 1,
		(const GLchar**)&source, (const GLint*)&fileLen); //a este shader le asigno codigo q he cargado de fichero (es un string q esta en memoria ppal)
	//1 --> juntamos cadenas de codigos (numero de cadenas q tiene mi codigo, en este caso solo una)), GLchar--> puntero a cadenas, 
	//longitud (puntero a un int)--> logitud de todas las cadenas, numero q he usado pa crear shaders, array de cadenas
	glCompileShader(shader); //ya tenemos codigo asignado --> lo compilamos
	delete[] source; //borramos cadena --> lo hace la funcion q lo ha reservado

	//Comprobamos que se compiló bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled); //queremos info concreta del shader --> si ha compilado o no
	if (!compiled) //si no ha compilado --> codigo de tratamiento de errores
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen); //si ha habido error lo primero es preguntar longitud de cadena de error
		char* logString = new char[logLen]; //reservar espacio para esa cadena de error
		glGetShaderInfoLog(shader, logLen, NULL, logString); //devuelvemela para string q he reservado y no te pases
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;
		glDeleteShader(shader);
		exit(-1);
	}
	return shader;
}

//unsigned int loadTex(const char* fileName) { return 0; }

unsigned int loadTex(const char* fileName)
{ 
	unsigned char* map; //como devuelve puntero sabemos q subimos textura a memoria principal
	unsigned int w, h;
	map = loadTexture(fileName, w, h); //si no da null devuelve puntero --> la funcion q reserva memoria es la q tiene q destruir --> la eliminamos mas abajo
	if (!map) //ver si punetro vale null (apunta direccion 0) -> quiere decir q no ha sido capaz de cargar textura
	{
		std::cout << "Error cargando el fichero: "
			<< fileName << std::endl;
		exit(-1);
	}

	//esto esta en memoria ppal, nos interesa q este en tarjeta grafica --> creamos objeto para subir la info
	unsigned int texId;
	GLfloat fLargest;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId); //solo la vamos a poder activar como 2D
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8 /*como estan almacenados en tarjeta grafica*/, w, h, 0, GL_RGBA /*como estan almacenados en punetro*/,
		GL_UNSIGNED_BYTE, (GLvoid*)map); //sube los datos
	//textura mutable: cuando puedo cambiar su tamaño

	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);		//cantidad máxima de anisotropía 
	//textura mutable: cuando puedo cambiar su tamaño

	delete[] map; //liberar todo el espacio

	//paso 12 --> liberar texturas una vez terminadas de usar

	//Para tratar aliasing --> midmap
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	//	GL_LINEAR_MIPMAP_LINEAR); //propiedad de la textura min filter (coords de textura de 2 fragments consecutivos difiere mucho): interpolacion trilineal creo
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //magfilter: caso opuesto --> no tiene suficiente resolucion
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//GL_CLAMP_TO_EDGE: si valores menor q 0 o mayor q uno --> quedate con el borde
	//nos queda cargar texturas

	//Devuelvo id de la textura q acabo de crear y para la q acabo de crear espacio
	return texId;
	
}

void renderFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpia frame buffer -->  lo q tenemos en back y buffer de profundidad (limpio lienzo sobre el q voy a escribir)
	//limpia color y profundidad --> limpia 2 buffers

	//----------------------------------------------------------------------------------------------------------------------------

	glUseProgram(program); //activar shader --> todo lo q se renderize a partir de este momento se renderiza con este programa
	//programa: shaders trabajando de forma conjunta (en nuestro caso el de vertices y el de fragmentos)

	glm::mat4 modelView = view * model;
	glm::mat4 modelViewProj = proj * modelView;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView)); //inversa de la transpuesta de la modelView
	//podemos hacer calculos dentro del shaders pero lo hacems aqui poara q no se hagan para cada hilo --> optimixzar
	//lo hacemos una vez y sirve para todas las variables
	//hilos: codigo del shader para cada vertice creo
	//De nuevo OpenGl no las conoce porq estan en memoria principal --> las subimos a tarjeta grafica

	if (uModelViewMat != -1) //identificador --> si vale -1 no lo usa --> no subo datos
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, //false permite transponer la matriz antes de subirla (o es q no permite transponerlas?) --> opengl codifica por columnas (lo normal es hacerlo por filas)
			&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
			&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
			&(normal[0][0]));
	if (uViewL != -1)
		glUniformMatrix4fv(uViewL, 1, GL_FALSE,
			&(view[0][0]));

	if (uPosicionL != -1)
		glUniform3fv(uPosicionL, 1, &(posicionL.x));
	if (uIntensidadL != -1)
		glUniform3fv(uIntensidadL, 1, &(intensidadL.x));
	
	//Texturas --> no se muy bn si iria aqui
	glActiveTexture(GL_TEXTURE0 + 12); //5: puerto donde esta la textura
	glBindTexture(GL_TEXTURE_2D, colorTexId);
	//glUniform1i(uColorTex, 0); --> ya o hemos subido antes en InitShader --> alli activamos programa y le dijimos en q puerto esta
	//ya no tenemos q subir puerto cada vez q renderizamos

	glActiveTexture(GL_TEXTURE0 + 13);
	glBindTexture(GL_TEXTURE_2D, emiTexId);
	//glUniform1i(uEmiTex, 1);

	glActiveTexture(GL_TEXTURE0 + 14);
	glBindTexture(GL_TEXTURE_2D, normalTexId);

	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_2D, specTexId);

	//Activar VAO --> lo siguiente q se va a renderizar esta configurado en vao
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
	//TRIANGLES --> interpretar indices como si fueran de triangulos --> q los coja de 3 en 3
	// esto lo vimos --> habia varuios tipos: GL_POINTS, GL_LINES...
	// tiene q leer el numero de triangulos * 3 (3 vertices cada triangulo)
	// Tipo de cada indices --> unsigned int  --> esto lo venmos en el BOX.h
	//void: opset -->
	
	

	//----------------------------------------------------------------------------------------------------

	//Para el nuevo cubo:
	//glm::mat4 t(1.0f); //para desplazar cubo
	//t[3].x = 3.0f;
	//Ya las hemos declarado arriba asiq ahora solo les damos nuevos valores
	//modelView = view * t * model2;
	//Como ahora va a rotar al rededor del otro cubo, metemos el desplazamiento en model2 directamente
	modelView = view * model2;
	modelViewProj = proj * modelView;
	normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1) 
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, 
			&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
			&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
			&(normal[0][0]));
	if (uViewL != -1)
		glUniformMatrix4fv(uViewL, 1, GL_FALSE,
			&(view[0][0]));

	if (uPosicionL != -1)
		glUniform3fv(uPosicionL, 1, &(posicionL.x));
	if (uIntensidadL != -1)
		glUniform3fv(uIntensidadL, 1, &(intensidadL.x));

	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, colorTexId);

	glActiveTexture(GL_TEXTURE0 + 13);
	glBindTexture(GL_TEXTURE_2D, emiTexId);

	glActiveTexture(GL_TEXTURE0 + 14);
	glBindTexture(GL_TEXTURE_2D, normalTexId);

	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_2D, specTexId);

	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);
	
	

	//----------------------------------------------------------------------------------------------------------------------------
	//Tercer cubo

	modelView = view * model3;
	modelViewProj = proj * modelView;
	normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE,
			&(modelView[0][0]));
	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE,
			&(modelViewProj[0][0]));
	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE,
			&(normal[0][0]));
	if (uViewL != -1)
		glUniformMatrix4fv(uViewL, 1, GL_FALSE,
			&(view[0][0]));

	if (uPosicionL != -1)
		glUniform3fv(uPosicionL, 1, &(posicionL.x));
	if (uIntensidadL != -1)
		glUniform3fv(uIntensidadL, 1, &(intensidadL.x));

	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, colorTexId);

	glActiveTexture(GL_TEXTURE0 + 13);
	glBindTexture(GL_TEXTURE_2D, emiTexId);

	glActiveTexture(GL_TEXTURE0 + 14);
	glBindTexture(GL_TEXTURE_2D, normalTexId);

	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_2D, specTexId);

	glDrawElements(GL_TRIANGLES, cubeNTriangleIndex * 3,
		GL_UNSIGNED_INT, (void*)0);


	

	glutSwapBuffers(); //cambiar buffers
	//si quitamos el swapbuffer se ve blanco porq front no cambia --> tiene siempre mismo color o algo asi
}
void resizeFunc(int width, int height)
{
	glViewport(0, 0, width, height);

	//Ajusta el aspect ratio al tamaño de la venta
	float aspectratio = (float(width) / float(height)); //Calculamos el aspect ratio

	proj[0].x = 1.0 / (aspectratio * (tan(30.0 * 3.141599 / 180.0)));
	proj[1].y = 1.0 / tan(30.0 * 3.141599 / 180.0);

	glutPostRedisplay();
}
void idleFunc()
{
	view[0] = right;
	view[1] = up;
	view[2] = -lookAt;
	view[3] = cop;

	view = glm::inverse(view);

	model = glm::mat4(1.0f);
	model2 = glm::mat4(1.0f);
	model3 = glm::mat4(1.0f);

	static float angle = 0.0f;
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.0001f;

	/*
	if (angle > 3.141592f * 2.0f) angle = 0.0;
	Sleep(50);
	*/

	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));

	//Para segundo objeto pintamos el mismo cubo en renderFunc y ahora creamos otra matriz model para darle otro movimiento:

	model2 = (glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 1.0f, 0.0))) * (glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, 0.0, 0.0))) * (glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 1.0f, 0.0))); //rota solo sobre su eje y

	//Para tercer cubo con curvas blazer:
	//se crean los puntos
	glm::vec3 a = glm::vec3();
	glm::vec3 b = glm::vec3();
	glm::vec3 c = glm::vec3();
	glm::vec3 d = glm::vec3();
	glm::vec3 e = glm::vec3();

	static float t = 0.0f;
	t = t + 0.005;
	if (t >= 1) {
		t = 0.0f;
	}

	glm::mat4 model3bezier = glm::mat4(1.0f);

	model3bezier[3].x = ((1 - t) * (1 - t) * (1 - t) * (1 - t)) * a.x + 3 * (t * t) * ((1 - t) * (1 - t) * (1 - t)) * b.x + 3 * t * ((1 - t) * (1 - t)) * c.x + 3 * (1 - t) * (t * t) * d.x + (t * t * t) * e.x;
	model3bezier[3].y = ((1 - t) * (1 - t) * (1 - t) * (1 - t)) * a.y + 3 * (t * t) * ((1 - t) * (1 - t) * (1 - t)) * b.y + 3 * t * ((1 - t) * (1 - t)) * c.y + 3 * (1 - t) * (t * t) * d.y + (t * t * t) * e.y;
	model3bezier[3].z = ((1 - t) * (1 - t) * (1 - t) * (1 - t)) * a.z + 3 * (t * t) * ((1 - t) * (1 - t) * (1 - t)) * b.z + 3 * t * ((1 - t) * (1 - t)) * c.z + 3 * (1 - t) * (t * t) * d.z + (t * t * t) * e.z;

	glm::mat4 model3rot = (glm::rotate(model3, angle, glm::vec3(0.0f, 1.0f, 0.0))) * (glm::translate(glm::mat4(1.0f), glm::vec3(-3.5f, 0.0, 0.0))) * (glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0))); //rota solo sobre su eje y
	//glm::mat4 model3scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	model3 = model3bezier * model3rot;

	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	static float angle = 0.1f;

	//glm::vec3 posLAux = glm::vec3(0.0);

	std::cout << "Se ha pulsado la tecla " << key << std::endl << std::endl;

	switch (key) {
		//Caso adelante --> eje z
	case 'w':
		cop += angle * lookAt;
		//matViewAux[3].z = matViewAux[3].z * angle;
		break;

		//Caso hacia atras --> eje z
	case 's':
		cop -= angle * lookAt;
		//matViewAux[3].z = matViewAux[3].z - 1.0f;
		break;

		//Caso hacia derecha --> eje x
	case 'd':
		cop += angle * right;
		//matViewAux[3].x = matViewAux[3].x + 1.0f;
		break;

		//Caso hacia la izquierda --> eje x
	case 'a':
		cop -= angle * right;
		//matViewAux[3].x = matViewAux[3].x - 1.0f;
		break;

		//Caso girar hacia derecha --> eje y
	case 'e':
		lookAt = glm::rotate(glm::mat4(1.0f), -0.05f, glm::vec3(0.0, 1.0f, 0.0)) * lookAt;
		right = glm::vec4(glm::cross(glm::vec3(lookAt), glm::vec3(up)), 0.0);
		//matViewAux = glm::rotate(matViewAux, -0.05f, glm::vec3(0.0, 1.0f, 0.0));
		break;

		//Caso girar hacia la izquierda --> eje y
	case 'q':
		lookAt = glm::rotate(glm::mat4(1.0f), 0.05f, glm::vec3(0.0, 1.0f, 0.0)) * lookAt;
		right = glm::vec4(glm::cross(glm::vec3(lookAt), glm::vec3(up)), 0.0);
		//matViewAux = glm::rotate(matViewAux, 0.05f, glm::vec3(0.0, 1.0f, 0.0));
		break;


		//Mover luz hacia delante
	case 'i':
		posicionL += glm::vec3(0.0, 0.0, -1.0f);
		break;

		//Mover luz hacia atras
	case 'k':
		posicionL += glm::vec3(0.0, 0.0, 1.0f);
		break;

		//Mover luz hacia derecha
	case 'l':
		posicionL += glm::vec3(1.0f, 0.0, 0.0);
		break;

		//Mover luz hacia izq
	case 'j':
		posicionL += glm::vec3(-1.0f, 0.0, 0.0);
		break;

		//Mover luz hacia arriba
	case 'p':
		posicionL += glm::vec3(0.0, 1.0f, 0.0);
		break;

		//Mover luz hacia abajo
	case 'o':
		posicionL += glm::vec3(0.0, -1.0f, 0.0);
		break;

		//Intensidad de la luz
	case 'u':
		if (intensidadL.x < 1) { //como x, y y z valen lo mismo, van a crecer y decrecer igual (al comparar x, sabemos que y y z valen lo mismo que x)
			//le ponemos un maximo de intensidad 15 por ejemplo
			intensidadL += glm::vec3(0.1f, 0.1f, 0.1f);
		}
		break;

	case 'y':
		if (intensidadL.x > 0) { //para que 
			intensidadL -= glm::vec3(0.1f, 0.1f, 0.1f);
		}
		break;

	case 0:
		exit(0);
		break;
	}

	//posicionL = posLAux * posicionL;
	glutPostRedisplay();
}
void mouseFunc(int button, int state, int x, int y) 
{
	static float angle = 0.1f;

	if (state == 0)
		std::cout << "Se ha pulsado el botón ";
	else
		std::cout << "Se ha soltado el botón ";

	if (button == 0)
	{
		std::cout << "de la izquierda del ratón " << std::endl;
		lookAt = glm::rotate(glm::mat4(1.0f), 0.05f, glm::vec3(0.0, 1.0f, 0.0)) * lookAt;
		right = glm::vec4(glm::cross(glm::vec3(lookAt), glm::vec3(up)), 0.0);
	}

	if (button == 1)
	{
		std::cout << "central del ratón " << std::endl;
		cop += angle * lookAt;
	}

	if (button == 2)
	{
		std::cout << "de la derecha del ratón " << std::endl;
		lookAt = glm::rotate(glm::mat4(1.0f), -0.05f, glm::vec3(0.0, 1.0f, 0.0)) * lookAt;
		right = glm::vec4(glm::cross(glm::vec3(lookAt), glm::vec3(up)), 0.0);
	}

	std::cout << "en la posición " << x << " " << y << std::endl << std::endl;
}









