//
// Created by A123 on 12/5/2024.
//

#ifndef EXPERIMENT2_H
#define EXPERIMENT2_H

#endif //EXPERIMENT2_H

# define  nodeMaxNumber 100

#include <map>
#include <vector>
#include <functional>
#include "tabulate/table.hpp"

struct Statue_node {
    std::vector<int> statue_pre; //
    std::vector<int> statue_next; // 终点
    std::vector<char> symbol; // 状态转换条件
};


std::string preprocessing_addJointMark(const std::string &line);

std::string preprocessing_nifix_2_postfix(const std::string &line);

int getRegexOpPriority(char);

std::string parseRegex(const std::string &lineRead);

void nifix_2_postfix();

void output_NFA_table();

void write_1row_into_table(int, char, int, tabulate::Table table);

void processFilebyLine(
    bool saveAsFile, const std::string &, const std::string &,
    const std::function<std::string(const std::string &)> &);
