#include "shader.h"

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
	string vertexCodeStr = readFile(vertexPath); 
	const GLchar* vShaderCode=vertexCodeStr.c_str();
	GLuint vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
	
	string fragmentCodeStr = readFile(fragmentPath);
	const GLchar* fShaderCode=fragmentCodeStr.c_str();
	GLuint fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);

	createProgram(vertex, fragment);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


void Shader::createProgram(GLuint vertexId, GLuint fragmentId) {
	GLint success;
	GLchar infoLog[512];

	this->program = glCreateProgram();
	glAttachShader(this->program, vertexId);
	glAttachShader(this->program, fragmentId);
	glLinkProgram(this->program);

	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		std::exit(1);
	}
}
	

void Shader::use() { 
	glUseProgram(this->program); 
}

GLuint Shader::compileShader(const GLchar* code, GLuint shader_type) {
	GLuint shaderId;
	GLint success;
	GLchar infoLog[512];

	shaderId = glCreateShader(shader_type);
	glShaderSource(shaderId, 1, &code, NULL);
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog;
		std::exit(1);
	}
	return shaderId;
}
	
std::string Shader::readFile(const GLchar *name) {
	std::string code;
	try {
		std::ifstream file;
		file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		file.open(name);
		std::stringstream fStream;
		fStream << file.rdbuf();
		file.close();
		code = fStream.str();
	} catch (std::ifstream::failure &e) {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
		std::exit(1);
	}
	return code;
}

