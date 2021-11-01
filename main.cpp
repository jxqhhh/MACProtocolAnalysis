#include <iostream>
#include <queue>

/**
 * 问题描述：
 * 假设目前有一个具有 n 个节点及 1 个网关的无线传感网络，节点的发送功耗为 100mW，监听功耗为 10mW，不考虑网关的功耗情况。传输时间固定为 1s。
 * 设 n = 3，设计如下 MAC 协议：每个节点以 1s 作为时间槽，分别间隔 3， 5， 7s 唤醒并监听信道 1s，网关间隔 1s 发送一段长度为 2s 的前导信号，其中包含了要询问的节点地址。若节点在唤醒过程中接收到了网关的前导信号并与自身地址相同，则在下个时间槽回传数据，否则进入休眠状态。
 * 计算全部节点传输完成 1 次时期望的总时间和总能耗。
 */

/**
 * 思路：
 * 作图可知，经过最多3*24=72s，全部节点必然传输完成，故采用搜索的方法遍历所有可能情况、对所有情况的时间和能耗加权求期望。
 */

typedef struct treeNode{
    float prob;
    int time; // start from 0 (i.e., this value is 0 for the root node)
    int totalEnergyConsumption; // the total energy consumption until .time (plus 100 if some node receive a request to it at this treeNode)
    bool isNode1Closed;
    bool isNode2Closed;
    bool isNode3Closed;
} treeNode;

int main() {
    treeNode root = {
        .prob = 1,
        .time = 0,
        .totalEnergyConsumption = 0,
        .isNode1Closed = false,
        .isNode2Closed = false,
        .isNode3Closed = false
    };
    std::queue<treeNode> nodeQueue;
    nodeQueue.push(root);
    float timeExpectation = 0, energyExpectation = 0;
    while(nodeQueue.size()>0) { // breadth-first search
        treeNode node = nodeQueue.front();
        nodeQueue.pop();
        if(node.isNode1Closed && node.isNode2Closed && node.isNode3Closed){
            timeExpectation += node.prob * node.time;
            energyExpectation += node.prob * node.totalEnergyConsumption;
            continue;
        }
        int time = node.time;
        time ++;
        while((time%4)!=0 && (time%6)!=0 && (time%8)!=0){
            time ++;
        }
        bool isNode1Candidate = (time%4==0) && !node.isNode1Closed,
                isNode2Candidate = (time%6==0) && !node.isNode2Closed,
                isNode3Candidate = (time%8==0) && !node.isNode3Closed; // a node is a candidate if it is monitoring the channel at time t
        int candidateNum = (isNode1Candidate?1:0) + (isNode2Candidate?1:0) + (isNode3Candidate?1:0);
        int numPossibleAddresses = (node.isNode1Closed?0:1) + (node.isNode2Closed?0:1) + (node.isNode3Closed?0:1);

        // Case 1: the gateway does not send any data at time t
        if(time%3 == 1) {
            treeNode childNode = node;
            childNode.time = time;
            childNode.totalEnergyConsumption += candidateNum * 10;
            nodeQueue.push(childNode);
            continue;
        }

        // Case 2: the gateway sends some data at time t
        treeNode childNode = node;
        childNode.time = time;
        if(isNode1Candidate){
            treeNode childNode1 = childNode;
            childNode1.prob /= numPossibleAddresses;
            childNode1.totalEnergyConsumption += (candidateNum * 10 + 100);
            childNode1.isNode1Closed = true;
            nodeQueue.push(childNode1);
        }
        if(isNode2Candidate){
            treeNode childNode2 = childNode;
            childNode2.prob /= numPossibleAddresses;
            childNode2.totalEnergyConsumption += (candidateNum * 10 + 100);
            childNode2.isNode2Closed = true;
            nodeQueue.push(childNode2);
        }
        if(isNode3Candidate){
            treeNode childNode3 = childNode;
            childNode3.prob /= numPossibleAddresses;
            childNode3.totalEnergyConsumption += (candidateNum * 10 + 100);
            childNode3.isNode3Closed = true;
            nodeQueue.push(childNode3);
        }
        if (numPossibleAddresses > candidateNum) {
            treeNode childNode0 = childNode;
            childNode0.prob *= (float(numPossibleAddresses-candidateNum))/(float(numPossibleAddresses));
            childNode0.totalEnergyConsumption += candidateNum * 10;
            nodeQueue.push(childNode0);
        }
    }
    std::cout << timeExpectation << " " << energyExpectation << std::endl;
    return 0;
}