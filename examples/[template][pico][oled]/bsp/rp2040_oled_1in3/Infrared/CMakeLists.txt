# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_Infrared_SRCS 变量
aux_source_directory(. DIR_Infrared_SRCS)

include_directories(../Config)

# 生成链接库
add_library(Infrared ${DIR_Infrared_SRCS})
target_link_libraries(Infrared PUBLIC Config)