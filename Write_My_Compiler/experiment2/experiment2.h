//
// Created by A123 on 12/5/2024.
//

#ifndef EXPERIMENT2_H
#define EXPERIMENT2_H

#endif //EXPERIMENT2_H

# define nodeMaxNumber 10
# define emptyState_char '@'
// #include <map>
#include <vector>
#include <functional>
#include <stack>
#include "tabulate/table.hpp"


struct StateNode{
    int num_of_Daddy = 0;  // 该节点的父节点的数量：方便后续按一定顺序遍历
    int stateNum = -1;  // 状态编号
    std::vector<char> nextState_conditions;  // 转换条件
    std::vector<StateNode*> nextStates;  // 根据转换条件而转换成的 下一个节点
};

struct TempResult {
    // 运行过程中的一些由多个节点结构体构成的中间结果，可以进一步抽象成此结构体
    // std::vector<Statue_node*> heads, tails; 不需要的，因为指向的是节点，而不是节点的结构体自己有多个分支
    StateNode *head;
    StateNode *tail;
};

std::string preprocessing_addJointMark(const std::string &line);

std::string preprocessing_nifix_2_postfix(const std::string &line);

int getRegexOpPriority(char);

std::string parseRegex(const std::string &line);

void operation_conjunction(std::stack<std::variant<char, TempResult>> &);
void operation_selection(std::stack<std::variant<char, TempResult>> &);
void operation_closure(std::stack<std::variant<char, TempResult>> &);
void operation_positiveClosure(std::stack<std::variant<char, TempResult>> &);
void operation_altinative(std::stack<std::variant<char, TempResult>> &);

void travel_StateMachine(StateNode &current, bool number_node_while_travelling, bool write_1row_into_table);
// void number_node_while_travelling(StateNode &current);
// void write_1row_into_table(int, char, int, tabulate::Table table);
void output_NFA_table();


void processFilebyLine(
    bool saveAsFile, const std::string &, const std::string &,
    const std::function<std::string(const std::string &)> &);