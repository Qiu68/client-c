cmake_minimum_required(VERSION 3.25)
project(stream_client C)
link_libraries(ws2_32)
set(CMAKE_C_STANDARD 11)

add_executable(stream_client main.c fileWrite.c command/ClientInitReqCommand.c utils/ByteTools.c utils/StringUtils.c utils/ThreadUtils.c utils/timeStamp.c command/ClientInitRespCommand.c command/response.h command/ClientInitReqCommand.h command/ClientInitRespCommand.h command/DesceribeReqCommand.h command/DesceribeReqCommand.c command/DescribeRespCommand.c command/DescribeRespCommand.h command/PauseReqCommand.h command/PauseReqCommand.c command/PauseRespCommand.h command/PauseRespCommand.c command/StartReqCommand.h command/StartReqCommand.c command/StartRespCommand.h command/StartRespCommand.c command/PingCommand.h command/PingCommand.c command/PongCommand.h command/PongCommand.c packet/PacketType.h packet/ClientInitReqPacket.h packet/ClientInitReqPacket.c packet/ClientInitRespPacket.h packet/ClientInitRespPacket.c packet/FramePacket.c packet/FramePacket.h client/connection/TcpConnection.c client/connection/UdpConnection.c client/task/TcpTask.c client/req/Pong.c client/Client.c client/task/UdpTask.c client/req/sendTcpMsg.c command/CommandLenght.h packet/PacketLength.h client/req/sendTcpMsg.c client/req/DescribeReq.c)

