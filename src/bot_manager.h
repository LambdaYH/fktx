#pragma once

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include <yaml-cpp/yaml.h>

#include "utils.h"
#include "logger.h"

struct BotInfo {
    std::string name;
    std::string enable_cmd;
    std::string disable_cmd;
    std::string log_path;
    __off_t offset;
    unsigned int error_count;
    bool status;
    BotInfo(const std::string &name, const std::string &enable_cmd, const std::string &disable_cmd, const std::string &log_path, __off_t offset = 0) : 
        name(name), 
        enable_cmd(enable_cmd),
        disable_cmd(disable_cmd),
        log_path(log_path), 
        offset(offset),
        error_count(0),
        status(false) {}
};

class BotManager {
public:
    BotManager(const std::filesystem::path &config_file) : BotManager(YAML::LoadFile(config_file)) {}

    BotManager(const YAML::Node &node) : bot_idx_(0) {
        auto cur_path = std::filesystem::current_path();
        auto data_path = cur_path / "data";
        if(!std::filesystem::exists(data_path) && !std::filesystem::create_directory(data_path))
            LOG_ERROR("创建数据路径失败");
        auto &bots = node["Bots"];
        error_limit_ = node["error_limit"].as<std::size_t>();
        for(std::size_t i = 0; i < bots.size(); ++i) {
            contains_.emplace_back(
                bots[i]["name"].as<std::string>(), 
                bots[i]["enable_cmd"].as<std::string>(), 
                bots[i]["disable_cmd"].as<std::string>(), 
                bots[i]["log_path"].as<std::string>());
        }
        total_sz_ = contains_.size();
        if(std::filesystem::exists(data_path / "bot_idx")) {
            std::ifstream(data_path / "bot_idx") >> bot_idx_;
            if(bot_idx_ >= total_sz_)
                bot_idx_ = 0;
        }
        for(int i = 0; i < total_sz_; ++i) {
            auto f = data_path / ("bot_" + std::to_string(i));
            if(std::filesystem::exists(f))
                std::ifstream(f) >> contains_[i].offset;
        }
        LOG_INFO("初始化完成，共{}个bot\n当前Bot为[{}]号", total_sz_, bot_idx_);
    }

    void Check() {
        int i = bot_idx_;
        do {
            if(!CheckLog(contains_[i].log_path, contains_[i].offset)) {
                if(!contains_[i].status) {
                    ExecCmd(contains_[i].enable_cmd);
                    if(bot_idx_ != i) {
                        LOG_INFO("已切换至[{}]", contains_[i].name);
                        bot_idx_ = i;
                    }
                    else
                        LOG_INFO("[{}]已恢复正常", contains_[i].name);
                    contains_[i].status = true;
                    contains_[i].error_count = 0;
                }
                break;
            }else {
                if(++contains_[i].error_count >= error_limit_) {
                    if(contains_[i].status) {
                        contains_[i].status = false;
                        ExecCmd(contains_[i].disable_cmd);
                    }
                    LOG_WARN("[{}]被风控了！！", contains_[i].name);
                    i = (i + 1) % total_sz_;
                    continue;
                }
                bot_idx_ = i;
                break;
            }
        }while(i != bot_idx_);
        if(i == total_sz_)
            bot_idx_ = 0;
    }

    void Save() const {
        auto cur_path = std::filesystem::current_path();
        auto data_path = cur_path / "data";
        std::ofstream(data_path / "bot_idx") << bot_idx_;
        for(int i = 0; i < total_sz_; ++i) 
            std::ofstream(data_path / ("bot_" + std::to_string(i))) << contains_[i].offset;
    }

private:
    std::vector<BotInfo> contains_;
    int total_sz_;
    int bot_idx_;
    std::size_t error_limit_;
};