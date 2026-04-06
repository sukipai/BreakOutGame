#include <Core/log.h>
#include <spdlog/sinks/stdout_color_sinks.h> // 用于控制台带颜色的输出

namespace Engine {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // 设置日志的格式 Pattern
        // %^: 开始颜色
        // %T: 时间戳 (HH:MM:SS)
        // %n: Logger 的名字 (ENGINE 或 APP)
        // %v: 实际的日志消息
        // %$: 结束颜色
        spdlog::set_pattern("%^[%T] %n: %v%$");

        // 初始化引擎日志器
        s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
        s_CoreLogger->set_level(spdlog::level::trace); // 设置最低显示级别

        // 初始化游戏应用日志器
        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger->set_level(spdlog::level::trace); 
    }

} // namespace Engine