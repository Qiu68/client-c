//
// Created by 24546 on 2023/10/20.
//
#include <stdio.h>
#include <pthread.h>

#include <stdlib.h>
#include <math.h>

#include "BitrateChangeState.h"
#include "PacketGroupDelay.h"
#include "DelayHistory.h"
#include "BandwidthUsage.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

extern pthread_mutex_t trendCalculaterMutex;
extern struct delayHistory *delayHistoryHead;

long long getSystemTimestamp();

int addDelayData(struct delayHistory *delayHistory);

int getDelayHistoryCount();

int delFirstNode();

double linearFitSlope(struct delayHistory *list);

char getChangeState(struct PacketGroupDelay *groupDelay);

char calculateGroup(struct PacketGroupDelay *groupDelay);

char getBitrateChange(char usage);

//char calculateLevel(struct bitrateChangeState *list,int count);




//通过延迟梯度样本计算网络趋势

// 过度使用时间阈值 默认值[10.0]
int OVERUSING_TIME_THRESHOLD = 100;
//平滑系数
double SMOOTHING_COEF = 0.9;
//网络状态趋势 计算增益系数
double THRESHOLD_GAIN = 4.0;
//trend阈值 最大偏差
double MAX_ADAPT_OFFSET = 15.0;
//trend阈值 上升计算系数
double K_UP = 0.0087;
//trend阈值 下降计算系数
double K_DOWN = 0.039;


//累计延时
double accumulatedDelay = 0;

//平滑延时
double smoothedDelay = 0;

//计算样本中第一个包组的到达时间
int firstArrivalTimeMs = -1;

//trend阈值  通过比较该值 判断是overusing还是underusing
double threshold = 12.5;

//传入的包组数量
int groupCount = 0;

//当前客户端的网络使用情况
char currentChangeState = HOLD;

int timeOverUsing = -1;
int overuserCounter = 0;

//上次计算的斜率
double prevTrend = 0;


//上次更新trend阈值的时间
int lastUpdateMs = -1;

//计算时间
long long startCalculateTime = 0;

long long calculater = 0ll;

struct bitrateChangeState {
    char stat;
    struct bitrateChangeState *next;
};


//通过多个网络趋势数据   计算出码率调整等级    返回结果[-100,100]  =0表示不调整  <0表示码率下调  >0表示码率上调
char calculateLevel(struct bitrateChangeState *list,int count) {
    if (count == 0) {
        return '0';
    }
    int changeCount = 0;
    struct bitrateChangeState *state;
    state = list;
    while(state != NULL){
        if (state->stat == INCR){
            changeCount++;
        }
        else{
            changeCount--;
        }
        state = state->next;
    }

    double levelTmp = (changeCount / count) * 100;
//    BigDecimal level = new BigDecimal(String.valueOf(changeCount)).divide(new
//    BigDecimal(String.valueOf(count)), 2, BigDecimal.ROUND_HALF_UP).multiply(new
//    BigDecimal("100"));
    int level = (int) levelTmp;
    if (level == 0 && count <= 10){
        return 50;
    }

    return (char)(level) - '48';
}


//通过心跳周期内的包组延时列表   计算出网络趋势
char calculate(struct PacketGroupDelay *groupDelays) {
    //根据发送时间排序
    if (groupDelays == NULL) {
        return 0;
    }
    //PacketGroupDelay.sort(groupDelays);
    char state = HOLD;
    //List<Byte> list = new ArrayList<Byte>();

    struct bitrateChangeState *stateList,*currState;
    stateList = currState = (struct bitrateChangeState*) malloc(sizeof (struct bitrateChangeState));
    currState->next = NULL;
    struct PacketGroupDelay *curr = groupDelays;
    int count = 0;
    while (curr != NULL) {
        state = calculateGroup(curr);
        curr = curr->next;
        currState->stat = state;
        currState->next = (struct bitrateChangeState*) malloc(sizeof (struct bitrateChangeState));
        currState = currState->next;
        currState->next = NULL;
        count++;
    }
    char level = calculateLevel(stateList,count);
    //        log.info("---size=" + groupDelays.size()+ "---level=" + level + "--list=" + StringUtils.join(list,","));
    return level;
}





//计算单组数据码率调整
char calculateGroup(struct PacketGroupDelay *groupDelay) {
    if (calculater == 0) {
        startCalculateTime = getSystemTimestamp();
    }
    if (getSystemTimestamp() - startCalculateTime > 2000) {
        //为减少网络波动影响     超过2秒 在需要重新计算延时趋势
        startCalculateTime = getSystemTimestamp();
    }
    return getChangeState(groupDelay);
}


void updateThreshold(double modifiedTrend, int arrivalTimeMs) {
    if (lastUpdateMs == -1) {
        lastUpdateMs = arrivalTimeMs;
    }
    if (abs(modifiedTrend) > threshold + MAX_ADAPT_OFFSET) {
        //trend阈值超出最大范围
        lastUpdateMs = arrivalTimeMs;
        return;
    }
    //获取阈值计算系数
    double k = abs(modifiedTrend) < threshold ? K_DOWN : K_UP;
    int kMaxTimeDeltaMs = 100;
    int deltaMs = MIN(arrivalTimeMs - lastUpdateMs, kMaxTimeDeltaMs);
    threshold += k * (abs(modifiedTrend) - threshold) * deltaMs;

    //SafeClamp   限制阈值在[6,600]之间
    if (threshold <= 6) {
        threshold = 6;
    }
    if (threshold >= 600) {
        threshold = 600;
    }

    lastUpdateMs = arrivalTimeMs;
}



int detect(double trend, int sendDeltaMs, int arrivalTimeMs) {
    int usage = NORMAL;
    if (groupCount < 2) {
        //当前样本数量不够
        return usage;
    }
    //实际使用中，由于trend 是一个非常小的值，会乘以包组数量和增益系数进行放大得到modifiedTrend
    double modifiedTrend = MIN(groupCount, 60) * trend * THRESHOLD_GAIN;
    if (modifiedTrend > threshold) {
        //持续时间超过100ms并且 trend值持续变大，认为此时处于 overuse 状态。
        if (timeOverUsing == -1) {
            // Initialize the timer. Assume that we've been over-using half of the time since the previous sample.
            timeOverUsing = sendDeltaMs / 2;
        } else {
            // Increment timer
            timeOverUsing += sendDeltaMs;
        }
        overuserCounter++;
        if (timeOverUsing > OVERUSING_TIME_THRESHOLD && overuserCounter > 1) {
            if (trend >= prevTrend) {
                timeOverUsing = 0;
                overuserCounter = 0;
                usage = OVERUSING;
            }
        }
    } else if (modifiedTrend < -threshold) {
        //认为此时处于underuse 状态
        timeOverUsing = 0;
        overuserCounter = 0;
        usage = UNDERUSING;
    } else {
        //认为此时处于normal 状态。
        timeOverUsing = 0;
        overuserCounter = 0;
        usage = NORMAL;
    }
    prevTrend = trend;
    updateThreshold(modifiedTrend, arrivalTimeMs);
//        log.info("TreadCalculater trend=" + trend + "-----modifiedTrend=" + modifiedTrend + "----threshold=" + threshold + "----usage=" + usage);
    return usage;
}


//放入包组样本   计算斜率  返回网络调整
char getChangeState(struct PacketGroupDelay *groupDelay) {

    pthread_mutex_lock(&trendCalculaterMutex);
    char result = currentChangeState;

    if (groupDelay == NULL) {
        pthread_mutex_unlock(&trendCalculaterMutex);
        return result;
    }


    if (firstArrivalTimeMs == -1) {
        firstArrivalTimeMs = groupDelay->arrivalTimeMs;
    }

    groupCount++;
    groupCount = MIN(groupCount, 1000);

    //包组延时
    int delayMs = groupDelay->recvDelta - groupDelay->sendDelta;
    accumulatedDelay += delayMs;

    //对包组延时做平滑处理    平滑延迟公式 = 平滑系数 * 平滑延迟 + (1 - 平滑系数) * 累积的延迟
    smoothedDelay = SMOOTHING_COEF * smoothedDelay + (1 - SMOOTHING_COEF) * accumulatedDelay;
    struct delayHistory *delayHistory = (struct delayHistory *) malloc(sizeof(struct delayHistory));
    delayHistory->key = groupDelay->arrivalTimeMs - firstArrivalTimeMs;
    delayHistory->value = smoothedDelay;

    if (getDelayHistoryCount() > 20) {
        //保持样本数量为20个
        delFirstNode();
    }
    if (getDelayHistoryCount() == 20) {
        //样本数量达到20个  开始计算斜率
        double trend = linearFitSlope(delayHistoryHead);
        if (trend != 0.0) {
            int usage = detect(trend, groupDelay->sendDelta, groupDelay->arrivalTimeMs);
            result = getBitrateChange(usage);
//                    log.info("TreadCalculater state=" + usage + "-----change=" + result + "---delayHistory size=" + delayHistory.size() + "------delayMs=" + delayMs);
        }
    }
    pthread_mutex_unlock(&trendCalculaterMutex);
    return result;

}



//判断网络过载状态   返回过载网络状态



//获取推送码率修改
char getBitrateChange(char usage) {

    if (usage == NORMAL) {
        if (currentChangeState == HOLD) {
            currentChangeState = INCR;
        }
    } else if (usage == OVERUSING) {
        if (currentChangeState != DECR) {
            currentChangeState = DECR;
        }
    } else if (usage == UNDERUSING) {
        currentChangeState = HOLD;
    }
    return currentChangeState;
}


//更新trend阈值


//通过最小二乘法求拟合出直线的斜率  根据斜率判断增长趋势
double linearFitSlope(struct delayHistory *list) {
    double sumX = 0;
    double sumY = 0;
    struct delayHistory *tmp;
    tmp = list;
    int listCount = 0;
    while (tmp != NULL) {
        sumX += tmp->key;
        sumY += tmp->value;
        tmp = tmp->next;
        listCount++;
    }
    double avgX = sumX / listCount;
    double avgY = sumY / listCount;

//直线方程y=bx+a的斜率b按如下公式计算:
// Compute the slope k = \sum (x_i-x_avg)(y_i-y_avg) / \sum (x_i-x_avg)^2
    double numerator = 0;
    double denominator = 0;
    tmp = list;
    while(tmp != NULL){
        numerator += (tmp->key - avgX) * (tmp->value - avgY);
        denominator += (tmp->key- avgX) * (tmp->value- avgX);
        tmp = tmp->next;
    }
    if (denominator == 0) {
        return 0.0;
    }

    return numerator / denominator;
}

