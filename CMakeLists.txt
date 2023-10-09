cmake_minimum_required(VERSION 3.25)
project(client_test)
link_libraries(ws2_32)
set(CMAKE_CXX_STANDARD 17)

add_executable(client_test main.c utils/ByteTools.c client/connection/TcpConnection.c client/connection/UdpConnection.c client/req/DescribeReq.c client/req/Pong.c client/req/sendTcpMsg.c client/task/TcpTask.c client/task/UdpTask.c client/Client.c command/ClientInitReqCommand.c command/ClientInitRespCommand.c command/DesceribeReqCommand.c command/DescribeRespCommand.c command/PauseReqCommand.c command/PauseRespCommand.c command/PingCommand.c command/PongCommand.c command/StartReqCommand.c command/StartRespCommand.c packet/ClientInitReqPacket.c packet/ClientInitRespPacket.c packet/FramePacket.c utils/StringUtils.c utils/ThreadUtils.c utils/timeStamp.c utils/ByteTools.c main.c main.c main.c utils/ListNodeTools.c client/pojo/Frame.h client/task/checkFramTask.h client/pojo/Frame.c client/task/checkFrameTask.c client/file/fileWrite.c)
