cmake_minimum_required(VERSION 3.22.1)
project(client_test)
set(CMAKE_CXX_STANDARD 17)
add_executable(client_test main.c utils/ByteTools.c client/connection/TcpConnection.c 
client/connection/UdpConnection.c client/req/DescribeReq.c client/req/Pong.c 
client/req/sendTcpMsg.c client/task/TcpTask.c client/Client.c command/ClientInitReqCommand.c 
command/ClientInitRespCommand.c command/DesceribeReqCommand.c command/DescribeRespCommand.c 
command/PauseReqCommand.c command/PauseRespCommand.c command/PingCommand.c
 command/PongCommand.c command/StartReqCommand.c command/StartRespCommand.c
  packet/ClientInitReqPacket.c packet/ClientInitRespPacket.c packet/FramePacket.c 
  utils/StringUtils.c utils/ThreadUtils.c utils/timeStamp.c utils/ByteTools.c main.c
   main.c main.c utils/ListNodeTools.c client/pojo/Frame.h client/task/checkFramTask.h
    client/pojo/Frame.c client/file/fileWrite.c client/req/PauseSeq.c client/req/PauseSeq.c 
    command/NackReqCommand.c command/NackReqCommand.h client/req/NackReq.c 
    trendline/BandwidthUsage.h trendline/BitrateChangeState.h client/pojo/PacketGroup.h 
    trendline/PacketGroupDelay.h trendline/PacketGroupDelay.c client/pojo/PacketGroup.c  
    log/log.c log/log.h client/task/checkFrameTask.c client/task/checkRetryTask.c
     client/task/UdpTask.c client/task/checkFrameTask.c client/task/checkFrameTask.c 
     client/task/checkFrameTask.c client/task/checkFrameTask.c client/task/checkFrameTask.c 
     trendline/TrendCalculater.c trendline/DelayHistory.c trendline/DelayHistory.h
        client/msg/MsgType.h client/msg/MsgQueue.c)
add_executable(send sendMsg.c)
add_executable(rcv rcvMsg.c)
