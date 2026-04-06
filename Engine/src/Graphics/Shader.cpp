#include <Shader.h>
#include <print>
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
    auto [vertexSrc, fragmentSrc, geometrySrc] = this->ParseShader(vertexPath, fragmentPath, geometryPath);

    ID = CreateShader(vertexSrc, fragmentSrc, geometrySrc);
}

std::tuple<std::string, std::string, std::string> Shader::ParseShader(const std::string& vertexFilePath, const std::string& fragmentFilePath, const std::string& geometryFilePath)  {
    auto readFile = [](const std::string& filepath) -> std::string {
        if (filepath.empty()) return "";
        std::ifstream stream(filepath);
        if (!stream.is_open()) {
            std::println("Error: file {} is not exists", filepath);
            return "";
        }
        std::string source((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        stream.close();
        return source;
    };

    return { 
        readFile(vertexFilePath), 
        readFile(fragmentFilePath), 
        readFile(geometryFilePath) 
    };
}

uint Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char * src = source.c_str();

    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::unique_ptr<char[]> message = std::make_unique<char[]>(length + 1);
        glGetShaderInfoLog(id, length, &length, message.get());
        
        std::string shaderType = "unknown";
        if (type == GL_VERTEX_SHADER) shaderType = "vertex";
        else if (type == GL_FRAGMENT_SHADER) shaderType = "fragment";
        else if (type == GL_GEOMETRY_SHADER) shaderType = "geometry"; 

        std::println("Failed to compile {} shader!", shaderType);
        std::println("{}", message.get());
        glDeleteShader(id);
        return 0;
    }

    return id;
}

uint Shader::CreateShader(const std::string & vertexShader, const std::string & fragmentShader, const std::string & geometryShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    unsigned int gs = 0;
    if (!geometryShader.empty()) {
        gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    if (gs != 0) {
        glAttachShader(program, gs);
    }

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    if (gs != 0) {
        glDeleteShader(gs);
    }

    return program;
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::deleteProgram() const {
    glDeleteProgram(ID);
}

int Shader::getUniformLocation(const std::string& name) const {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(ID, name.c_str());

    if (location == -1) {
        std::println("Warning: uniform '{}' doesn't exist or is not used!", name);
    }

    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(getUniformLocation(name), (int) value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setMatrix4f(const std::string& name, const glm::mat4& matrix) const {
    uint transformLoc = getUniformLocation(name);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec2f(const std::string& name, const glm::vec2& vec) const {
    uint loc = getUniformLocation(name);
    glUniform2fv(loc, 1, glm::value_ptr(vec));
}

void Shader::setVec3f(const std::string& name, const glm::vec3& vec) const {
    uint loc = getUniformLocation(name);
    glUniform3fv(loc, 1, glm::value_ptr(vec));
}

void Shader::setVec4f(const std::string& name, const glm::vec4& vec) const {
    uint loc = getUniformLocation(name);
    glUniform4fv(loc, 1, glm::value_ptr(vec));
}

Shader::~Shader() {
    this->deleteProgram();
}