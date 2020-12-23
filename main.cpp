#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

#include <thread>
#include <functional>
#include <vector>
#include <string>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "shader.h"
// Window dimensions

#include <math.h>

#define WIDTH 1400
#define HEIGHT 800
#define MAX_ITERATION 1000

#define PI 3.14159265

Shader gShaderProgram;
unsigned int gTexture;
GLuint VBO, VAO, EBO;;
GLFWwindow *window;

unsigned char gScreenBuffer[HEIGHT*WIDTH*3];

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Colour;

Colour gPalette[MAX_ITERATION];

int setupMain(void);
void setupPalette(void);
void setupTextures(void);
void setScreenBuffer(void);
inline double lerp(double v0, double v1, double t);
inline Colour lerpColour(Colour v0, Colour v1, double t);
void setNewTextureData(void);
void setScreenBufferWorker(int start, int number);
void setScreenBufferThreads(void);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setMandelbrotUniformsInit(void);
void setMandelbrotUniformsUpdate(void);

double zoom = 1.0;
double xoffset = 0.0;
double yoffset = 0.0;
int numThreadsSupported;
bool mousedown;
class Threads {
public:
	explicit Threads(size_t numThreads) {
		start(numThreads);
	}
	~Threads() {
		stop();
	}
	bool running = true;
private:
	std::vector<std::thread> _Threads;

	void start(size_t numThreads) {
		int eachthread_rows = HEIGHT / numThreads;
		int remainder = HEIGHT % numThreads;
		int onrow = 0;
		for (auto i = 0; i < numThreads; i++)
		{
			int numrows;

			if (i != numThreads - 1) {
				numrows = eachthread_rows;

			}
			else {
				numrows = eachthread_rows + remainder;
			}

			_Threads.emplace_back(setScreenBufferWorker, onrow, numrows);
			//std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
			//_Threads[i].join();
			//std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			//std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
			//std::cout<<"thread " << _Threads[i].get_id()<<  " done in " << time_span.count() * 1000 << " ms" << std::endl;
			onrow += eachthread_rows;
		}
		for (auto j = 0; j < _Threads.size(); j++) {
			_Threads[j].join();

			//std::cout << "joined Thread with ID: " << _Threads[j].get_id() << std::endl;
		}

	}
	void stop() {

	}
};
class ThreadPool {
public:
	explicit ThreadPool(size_t numThreads) {
		start(numThreads);
	}
	~ThreadPool() {
		stop();
	}
	bool running = true;
	void compute() {
		for (int i = 0; i < _Threads_Finished.size(); i++) {
			_Threads_Finished[i] = false;
		}
		for (int i = 0; i < _Threads_Finished.size(); i++) {
			while (!_Threads_Finished[i]);
		}
	}
	std::vector<bool> _Threads_Finished;
private:
	std::vector<std::thread> _Threads;
	
	void start(size_t numThreads) {
		
		int eachthread_rows = HEIGHT / numThreads;
		int remainder = HEIGHT % numThreads;
		int onrow = 0;
		for (auto i = 0; i < numThreads; i++)
		{
			_Threads_Finished.push_back(true);
			int numrows;

			if (i != numThreads - 1) {
				numrows = eachthread_rows;
				
			}
			else {
				numrows = eachthread_rows + remainder;
			}
			_Threads.emplace_back([this, onrow, numrows, i]() {
				while (true) {
					while (this->_Threads_Finished[i]);
					setScreenBufferWorker(onrow, numrows);
					this->_Threads_Finished[i] = true;
				}
			});//(setScreenBufferWorker, onrow, numrows);
			onrow += eachthread_rows;
		}
		
		
	}
	void stop() {

	}
};
void setScreenBufferThreadPool(ThreadPool& pool);
// The MAIN function, from here we start the application and run the game loop
int main()
{
	numThreadsSupported = std::thread::hardware_concurrency();
	//ThreadPool pool(numThreadsSupported);
	std::cout << numThreadsSupported <<" hardware threads"<<std::endl;
	if (setupMain() == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//setupTextures();
	setupPalette();
	
	//std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
	//setScreenBufferThreadPool(pool);
    //setScreenBufferThreads();
	//std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	//std::cout << "done in "<< time_span.count()*1000<<" ms" << std::endl;
	//setNewTextureData();
    // Game loop
	gShaderProgram.use();
	setMandelbrotUniformsInit();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents( );
        
        //glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
        //glClear( GL_COLOR_BUFFER_BIT );
        
		
		
		//glBindTexture(GL_TEXTURE_2D, gTexture);
        
		//std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
		
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glBindVertexArray( 0 );
		//std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
		//std::cout << "done in "<< time_span.count()*1000<<" ms" << std::endl;
        glfwSwapBuffers( window );
    }
    
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
int setupMain() {
	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "fractals", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);


	gShaderProgram.load("MandelbrotVert.shader", "MandelbrotFrag.shader");


	// Set up vertex data (and buffer(s)) and attribute pointers
	float vertices[] = {
     // position            tex coordinates
	 1.0f,  1.0f, 0.0f,      1.0f, 0.0f,   // top right
	 1.0f, -1.0f, 0.0f,      1.0f, 1.0f,   // bottom right
	 -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,   // bottom left
	 -1.0f,  1.0f, 0.0f,     0.0f, 0.0f    // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texcoords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
	
	
}
void setupTextures() {


	glGenTextures(1, &gTexture);
	glBindTexture(GL_TEXTURE_2D, gTexture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
}
void setNewTextureData() {
	glBindTexture(GL_TEXTURE_2D, gTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, gScreenBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
}
inline double lerp(double v0, double v1, double t) {
	return (1 - t) * v0 + t * v1;
}
inline Colour lerpColour(Colour v0, Colour v1, double t) {
	Colour c =  { 
		(unsigned char)lerp((double)v0.red,(double)v1.red,t),
		(unsigned char)lerp((double)v0.green,(double)v1.green,t),
		(unsigned char)lerp((double)v0.blue,(double)v1.blue,t)
	};
	return c;
}
void setScreenBuffer() {
	int onbyte = 0;
	for (int Py = 0; Py < HEIGHT; Py++)
	{
		for (int Px = 0; Px < WIDTH; Px++)
		{
			double x0 = (((double)Px / WIDTH)*3.5) - 2.5; // mandelbrot x between -2.5 and 1
			double y0 = (((double)Py / HEIGHT)*2.0) - 1.0; // mandelbrot y between -1 and 1
			double x = 0;
			double y = 0;
			int iteration = 0;
			
			double x2 = 0;
			double y2 = 0;
			while (x2 + y2 <= 4 && iteration < MAX_ITERATION) {
				y = 2 * x * y + y0;
				x = x2 - y2 + x0;
				x2 = x * x;
				y2 = y * y;
				iteration++;
			}

			Colour c = gPalette[iteration];
			gScreenBuffer[onbyte] = c.red;
			gScreenBuffer[onbyte + 1] = c.green;
			gScreenBuffer[onbyte + 2] = c.blue;
			onbyte += 3;
		}
	}
}
void setScreenBufferWorker(int start, int number) {
	//std::cout << "thread: " << std::this_thread::get_id() << " start " << start << " number " << number << std::endl;
	int onbyte = WIDTH * start * 3;
	//std::cout << "thread: " << std::this_thread::get_id() << " onbyte " << onbyte << std::endl;
	for (int Py = start; Py < start + number; Py++)
	{
		//std::cout << "thread: " << std::this_thread::get_id() << " row " << Py << std::endl;
		for (int Px = 0; Px < WIDTH; Px++)
		{
			double x0 = ((((double)Px / WIDTH)*(3.5*zoom)) - 2.5*zoom)+xoffset;     // mandelbrot x between -2.5 and 1
			double y0 = ((((double)Py / HEIGHT)*(2.0*zoom)) - (1.0*zoom))+yoffset;    // mandelbrot y between -1 and 1
			double x = 0;
			double y = 0;
			int iteration = 0;
			//std::cout << "ID: " << std::this_thread::get_id() << " x0: " << x0 << " y0: " << y0 << std::endl;
			double x2 = 0;
			double y2 = 0;
			while (x2 + y2 <= 4 && iteration < MAX_ITERATION) {
				y = 2 * x * y + y0;
				x = x2 - y2 + x0;
				x2 = x * x;
				y2 = y * y;
				iteration++;
			}
			Colour c = gPalette[iteration];
			gScreenBuffer[onbyte] = c.red;
			gScreenBuffer[onbyte + 1] = c.green;
			gScreenBuffer[onbyte + 2] = c.blue;
			onbyte += 3;
		}
	}
}

void setMandelbrotUniformsInit() {
	/*for (int i = 0; i < MAX_ITERATION; i++) {
		glm::vec3 colour = { (GLfloat)gPalette[i].red,(GLfloat)gPalette[i].green,(GLfloat)gPalette[i].blue };
		std::string name = "palette[" + std::to_string(i) + "]";
		gShaderProgram.setVec3(name, colour);
		//GLint location = glGetUniformLocation(gShaderProgram.ID, name.c_str());
		//glUniform4f(gShaderProgram.ID, location, gPalette[i].red, gPalette[i].green, gPalette[i].blue);
	}*/
	glm::dvec2 dims = { WIDTH,HEIGHT };
	gShaderProgram.setDVec2("screendims",dims);
	glm::dvec4 TLBR = {
		((0 * (3.5*zoom)) - 2.5*zoom) + xoffset,
		((0 * (2.0*zoom)) - (1.0*zoom)) + yoffset,
		((1 * (3.5*zoom)) - 2.5*zoom) + xoffset,
		((1 * (2.0*zoom)) - (1.0*zoom)) + yoffset
	};
	gShaderProgram.setDVec4("TLBR", TLBR);
}
void setMandelbrotUniformsUpdate() {
	glm::dvec4 TLBR = {
		((0 * (3.5*zoom)) - 2.5*zoom) + xoffset,
		((0 * (2.0*zoom)) - (1.0*zoom)) + yoffset,
		((1 * (3.5*zoom)) - 2.5*zoom) + xoffset,
		((1 * (2.0*zoom)) - (1.0*zoom)) + yoffset
	};
	gShaderProgram.setDVec4("TLBR", TLBR);
}
void setupPalette() {
	for (int i = 0; i < MAX_ITERATION; i++)
	{
		double fraction = (double)i/(double)MAX_ITERATION;
		Colour col;
		gPalette[i].red = (unsigned char)(255.0 * sin(((2 * PI)*(fraction))/0.5));
		gPalette[i].green = (unsigned char)(255.0 * sin((( PI)*(fraction + 0.66)) / 0.6));
		gPalette[i].blue = (unsigned char)(255.0 * sin(((2 * PI)*(fraction + 0.33)) / 0.8));

	}
}
void setScreenBufferThreadPool(ThreadPool& pool) {
	//Threads t(numThreadsSupported);
	pool.compute();
}
void setScreenBufferThreads() {
	Threads t(numThreadsSupported);
}
double lastx;
double lasty;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << "x " << xpos << " y " << ypos << std::endl;
	double deltax = ((xpos - lastx)/WIDTH)*(3.5*zoom);
	double deltay = ((ypos - lasty)/HEIGHT)*(3.5*zoom);
	if (mousedown) {
		xoffset -= deltax;
		yoffset -= deltay;
		//setScreenBufferThreads();
		//setNewTextureData();
		setMandelbrotUniformsUpdate();
	}
	lastx = xpos;
	lasty = ypos;
	
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mousedown = true;
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mousedown = false;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//std::cout << yoffset << std::endl;

	if (yoffset < 0) {
		for(int i=0; i<yoffset; i++)
			zoom *= 1.1;
	}
	else {
		for (int i = 0; i < yoffset; i++)
			zoom *= 0.9;
	}
	//setScreenBufferThreads();
	//setNewTextureData();
	setMandelbrotUniformsUpdate();

}

