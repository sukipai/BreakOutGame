#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> 

namespace Engine {

    class Log {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

} // namespace Engine

// 引擎内部使用的日志宏 (比如在 ResourceManager 或者 SpriteRenderer 里用)
#define ENGINE_CORE_TRACE(...)    ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_CORE_INFO(...)     ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_CORE_WARN(...)     ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_CORE_ERROR(...)    ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CORE_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// 客户端(游戏)使用的日志宏 (比如在 Game.cpp 里用)
#define APP_TRACE(...)            ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)             ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define APP_WARN(...)             ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)            ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)         ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)