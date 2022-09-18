#include <signal.h>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <unistd.h>

#include "yaml-cpp/yaml.h"

#include "utils.h"
#include "bot_manager.h"
#include "logger.h"

std::unique_ptr<BotManager> bot_manager;

void sigint_handler(int sig) {
    LOG_INFO("正在保存数据...");
    if(bot_manager)
        bot_manager->Save();
    LOG_INFO("正在关闭程序...");
    exit(0);
}

int main() {
    std::filesystem::path root_path = std::filesystem::current_path();
    auto config = root_path / "config.yml";
    LOG_INIT((root_path / "logs" / "fxtx.log").string(), "INFO");
    YAML::Node node = YAML::LoadFile(config);
    std::size_t interval = node["interval"].as<std::size_t>() * 60;
    bot_manager = std::make_unique<BotManager>(node);
    struct sigaction sa{};
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, nullptr);
    while(true) {
        bot_manager->Check();
        sleep(interval);
    }
}