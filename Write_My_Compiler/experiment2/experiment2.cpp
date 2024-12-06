//
// Created by A123 on 12/5/2024.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cctype>  // 字符处理
#include <stack>
#include "tabulate/table.hpp"
#include "experiment2.h"

Statue_node node[nodeMaxNumber];
std::stack<Statue_node> statueStack;

void addJointMark() {
    // 在需要的地方，添加连接符号
    std::string input_file = "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/regexNifix.txt";
    std::string output_file = "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/regexNifix_jointMark.txt";

    std::ifstream file(input_file);
    // 检查文件是否成功打开
    if (!file.is_open()) {
        std::cerr << "Unable to open file!" << std::endl;
        return;
    }

    char ch;  // 逐个扫描到的字符
    std::string line;  // 存储处理过的字符
    std::ofstream outfile(output_file);
    while (file.get(ch)) {
        if (isalpha(ch) && line.back() != '('
            && line.back() != '|' && line.back() != '/'
            && line.back() != '\0') {
            std::cout << "hh"<< std::endl;
            line += '.';
            line += ch;
        }
        else if(ch == '\n') {
            outfile << line << std::endl;
            line = "";
        }else {
            line += ch;
        }

    }
    outfile << line << std::endl;
    outfile.close();

}


void output_NFA_table() {
    tabulate::Table table;
    std::cout << std::endl << "[NFA Table]:" << std::endl;
    table.add_row({"Statue\\symbol", "a", "b","..."}); // 添加表头
    table.add_row({"1", "{2}", "{3}","..."}); // 添加表头
    table.add_row({"...", "...", "...","..."}); // 添加表头

    // for (auto & i : node) {
    //     // if (i.symbol[1] == '\0') break;  // symbol中没有任何内容，就不要输入了
    //     table.add_row({
    //         std::to_string(i.statue_pre[0]),
    //         std::string(1, i.symbol[0]),
    //         std::to_string(i.statue_next[0])
    //     });
    // }

    table.format().border_color(tabulate::Color::yellow).font_color(tabulate::Color::yellow);
    // table[0][0].format().font_style({tabulate::FontStyle::bold}); // 为特定单元格设置粗体
    std::cout << table << std::endl;


}


void parseRegex(char ch, int &statue) {
    // 解析正则表达式，并分别存储在结构体数组node中
    if (isalpha(ch)) {
        node[statue].symbol.push_back(ch);
        node[statue].statue_pre.push_back(statue);
        node[statue].statue_next.push_back(statue+1);
        statue++;
    }else if (ch == '@') return;

}