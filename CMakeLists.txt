cmake_minimum_required(VERSION 3.15)

project(UserImageManager VERSION 1.0 LANGUAGES CXX)

# Util
add_subdirectory(Util)
#SWidget模块
add_subdirectory(SWidget)
#日志模块
add_subdirectory(ThirdParty/QsLog)
#Jwt模块
add_subdirectory(SJwt)
# 客户端
add_subdirectory(UserImageManager-Client)
# 服务器
add_subdirectory(UserImageManager-Server)