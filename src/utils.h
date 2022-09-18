#pragma once

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <errno.h>
#include <stdlib.h>

constexpr auto kErrorMsg = "Protocol -> sendPacket msg error: 46";

inline void ExecCmd(const std::string &cmd) {
    if(cmd.empty())
        throw std::runtime_error("cmd cannot be empty");
    if(system(cmd.c_str()) == -1)
        throw std::runtime_error("fail to exec" + cmd);
}

/**
 * @brief 检查日志，若风控返回true
 * 
 * @param file_path 日志文件路径
 * @param offset 偏移量
 * @return true 已风控
 * @return false 未风控
 */
inline bool CheckLog(const std::string &file_path, __off_t &offset) {
    auto fd = open(file_path.c_str(), O_RDONLY);
    if(fd == -1)
        throw std::runtime_error(strerror(errno));
    auto len = lseek(fd, 0, SEEK_END);
    if(len == 0 || offset == len + 1)
        return false;
    if(offset >= len + 10) // 偏移量“远超”文件长度后，判定为新文件
        offset = 0;   
    auto file_addr = (char *)mmap(nullptr, len, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    auto ret = strstr(file_addr + offset, kErrorMsg);
    munmap(file_addr, len);
    offset = len + 1;
    return ret;
}