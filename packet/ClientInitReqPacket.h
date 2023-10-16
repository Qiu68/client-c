//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_CLIENTINITREQPACKET_H
#define STREAM_CLIENT_CLIENTINITREQPACKET_H

#endif //STREAM_CLIENT_CLIENTINITREQPACKET_H

struct clientInitReqPacket{
  char type;
  char clientHost[4];
  int clientPort;
};