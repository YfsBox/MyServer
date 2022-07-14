#include "../include/Httpdata.h"

void Httpdata::setTimer(std::shared_ptr<TimerNode> tnode) {
    tnode_ = tnode;
}
//对于单个节点的作用
void Httpdata::closeTimer() {
    if(tnode_.lock()){//lock用于将weak转化成shared(不为空)
        std::shared_ptr<TimerNode> tmp_node(tnode_.lock());
        
        tmp_node->setDeleted(false);
        tmp_node.reset();
    }
}