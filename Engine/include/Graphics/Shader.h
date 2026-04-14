#ifndef LEARNOPENGL_SHADER_H
#define LEARNOPENGL_SHADER_H

#include <string>
#include <tuple>
#include <glm/glm.hpp>
#include <unordered_map>

using uint = unsigned int;

class Shader {
private:
    uint ID;

    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    std::tuple<std::string, std::string, std::string> ParseShader(const std::string& vertexFilePath, const std::string& fragmentFilePath, const std::string& geometryFilePath);

    uint CompileShader(unsigned int type, const std::string& source);

    uint CreateShader(const std::string & vertexShader, const std::string & fragmentShader, const std::string & geometryShader);

    int getUniformLocation(const std::string& name) const;

public:
    Shader(const std::string &vertexPath, const std::string & fragmentPath, const std::string &geometryPath = "");

    ~Shader();

    void use() const;

    uint getID() const { return ID; }

    void deleteProgram() const;

    void setInt(const std::string& name, int value) const;
    void setBool(const std::string& name, bool value) const;
    void setFloat(const std::string &name, float value) const;
    void setMatrix4f(const std::string& name, const glm::mat4& matrix, uint count = 1) const;
    void setVec2f(const std::string& name, const glm::vec2& vec, uint count = 1) const;
    void setVec3f(const std::string& name, const glm::vec3& vec, uint count = 1) const;
    void setVec4f(const std::string& name, const glm::vec4& vec, uint count = 1) const;
};

#endif