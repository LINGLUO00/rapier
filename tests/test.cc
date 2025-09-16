#include "../src/log.h"
#include <cassert>
#include <cstring>
#include <iostream>

using namespace rapier;

int main() {
    /* ==== 1. 枚举转字符串测试 ==== */
    assert(std::strcmp(LogLevel::ToString(LogLevel::DEBUG), "DEBUG") == 0);
    assert(std::strcmp(LogLevel::ToString(LogLevel::INFO ), "INFO" ) == 0);
    assert(std::strcmp(LogLevel::ToString(LogLevel::WARN ), "WARN" ) == 0);
    assert(std::strcmp(LogLevel::ToString(LogLevel::ERROR), "ERROR") == 0);
    assert(std::strcmp(LogLevel::ToString(LogLevel::FATAL), "FATAL") == 0);

    /* ==== 2. Logger 基本属性测试 ==== */
    auto logger = std::make_shared<Logger>("unit_logger");
    assert(logger->getName() == "unit_logger");

    std::cout << "All rapier basic unit tests passed.\n";
    return 0;
}