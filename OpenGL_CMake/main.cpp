#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void GLAPIENTRY messageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

static GLuint compileShader(GLenum type, const GLchar* source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		const char* message = (const char*) alloca(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, (char*) message);

		// Provide the infolog in whatever manor you deem best.
		std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " Shader" << std::endl;
		std::cerr << "Detailed error message: " << message << std::endl;
		std::cerr << "Press <enter> to abort..." << std::endl;
		std::cin.get();

		glDeleteShader(shader); // Don't leak the shader.
		// Exit with failure.
		exit(1);
	}

	return shader;
}

static GLuint createShader(const GLchar* vertex, const GLchar* fragment)
{
	GLuint program = glCreateProgram();
	GLuint vs = compileShader(GL_VERTEX_SHADER, vertex);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragment);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	GLint isLinked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		const GLchar* message = (const char*) alloca(sizeof(GLchar) * maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, (char*) message);

		std::cerr << "Failed to link shader program: " << message << std::endl;
		std::cerr << "Press <enter> to abort..." << std::endl;
		std::cin.get();
		exit(1);
	}

	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	glDetachShader(program, vs);
	glDetachShader(program, fs);

	return program;
}

GLFWwindow* createWindow(GLuint width, GLuint height, const char* title)
{
	/* Initialize the library */
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		std::cerr << "Press <enter> to abort..." << std::endl;
		std::cin.get();
		exit(1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		std::cerr << "Press <enter> to abort..." << std::endl;
		std::cin.get();
		exit(1);
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to init GLEW" << std::endl;
		std::cerr << "Press <enter> to abort..." << std::endl;
		std::cin.get();
		exit(1);
	}

	return window;
}

int main(void)
{
	GLFWwindow* window = createWindow(800, 600, "Triangle example");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(messageCallback, nullptr);

	std::cout << glGetString(GL_VERSION) << std::endl;

	GLfloat positions[] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*) 0);
	glEnableVertexAttribArray(0);

	const char* vs = R"(
		#version 330 core
		layout(location = 0) in vec2 position;
		void main()
		{
			gl_Position = vec4(position, 0.0f, 1.0f);
		}
	)";

	const char* fs = R"(
		#version 330 core
		out vec4 color;
		void main()
		{
			color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	)";

	GLuint shader = createShader(vs, fs);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	std::cout << "Press <enter> to terminate..." << std::endl;
	std::cin.get();
	return 0;
}