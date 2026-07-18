#pragma once

#ifdef _DEBUG
#include <iostream>
#define LOG(message) std::cout << message << '\n'
#else
#define LOG(a)
#endif

#define LOG_ERROR(message) LOG("[Error] " << message)
#define LOG_INFO(message) LOG("[Info] " << message)
