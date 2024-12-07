//
// Created by A123 on 12/5/2024.
//

#ifndef EXPERIMENT2_H
#define EXPERIMENT2_H

#endif //EXPERIMENT2_H

# define  nodeMaxNumber 100

#include <vector>
#include <functional>

struct Statue_node{
    std::vector<int> statue_pre;  //
    std::vector<int> statue_next;    // 终点
    std::vector<char> symbol; // 状态转换条件
};

std::string addJointMark(const std::string& line, const std::string& file_output);
void nifix_2_postfix();
void output_NFA_table();
void parseRegex(char, int&);
void processFilebyLine(
    const std::string&, const std::string&,
    std::function<std::string(const std::string&, const std::string&)>);
