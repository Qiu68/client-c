cmake_minimum_required(VERSION 3.25)
project(stream_client C)

set(CMAKE_C_STANDARD 11)

add_executable(stream_client main.c fileWrite.c command/ClientInitReqCommand.c utils/ByteTools.c utils/StringUtils.c utils/ThreadUtils.c utils/timeStamp.c command/ClientInitRespCommand.c command/response.h command/ClientInitReqCommand.h command/ClientInitRespCommand.h command/DesceribeReqCommand.h command/DesceribeReqCommand.c command/DescribeRespCommand.c command/DescribeRespCommand.h command/PauseReqCommand.h command/PauseReqCommand.c)
