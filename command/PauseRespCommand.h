//
// Created by 24546 on 2023/9/19.
//

#ifndef STREAM_CLIENT_PAUSERESPCOMMAND_H
#define STREAM_CLIENT_PAUSERESPCOMMAND_H

struct PauseRespInfo{
    char command;
    int seq;
    char success;
};

#endif //STREAM_CLIENT_PAUSERESPCOMMAND_H
