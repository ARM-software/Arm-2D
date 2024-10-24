# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_Icm20948_SRCS 变量
aux_source_directory(. DIR_Icm20948_SRCS)

include_directories(../Config)

# 生成链接库
add_library(Icm20948 ${DIR_Icm20948_SRCS})
target_link_libraries(Icm20948 PUBLIC Config)