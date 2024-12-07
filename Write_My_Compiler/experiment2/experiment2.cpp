//
// Created by A123 on 12/5/2024.
//
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>  // 函数传参
#include <string>
#include <cctype>  // 字符处理
#include <stack>
#include <map>
#include "tabulate/table.hpp"
#include "experiment2.h"

Statue_node node[nodeMaxNumber];
std::stack<Statue_node> statueStack;
std::map<char, int> operator_priority;


void processFilebyLine(
    bool saveAsFile, const std::string &filePath_input,
    const std::string &filePath_output, std::function<std::string(const std::string &)> lineProcessor) {
    // 打开要处理的文件 and 检查是否打开
    std::ifstream fileRead(filePath_input);
    if (!fileRead.is_open()) {
        std::cerr << "Failed to open the file: " << filePath_input << std::endl;
        return;
    }


    std::string lineRead; // 从文件中读取出来的行

    if (saveAsFile) {
        // 如果需要保存为文件
        std::ofstream fileWrited(filePath_output);
        if (!fileWrited.is_open()) {
            std::cerr << "Failed to open the file: " << filePath_output << std::endl;
            return;
        }

        while (std::getline(fileRead, lineRead)) {
            // 逐行读取
            if (lineRead.empty()) continue;
            std::string line_to_write = lineProcessor(lineRead); // 调用传入的函数处理每行
            fileWrited << line_to_write << std::endl;
        }
        fileWrited.close();
    } else {
        while (std::getline(fileRead, lineRead)) {
            // 逐行读取
            if (lineRead.empty()) continue;
            std::string line_to_write = lineProcessor(lineRead); // 调用传入的函数处理每行
        }
    }


    fileRead.close(); // 关闭文件
}

std::string addJointMark(const std::string &line) {
    // 在需要的地方，添加连接符号

    std::string line_to_write; // 准备写入的行

    for (auto ch: line) {
        if (
            isalpha(ch)
            && line_to_write.back() != '('
            && line_to_write.back() != '|'
            && line_to_write.back() != '/'
            && line_to_write.back() != '\0') {
            line_to_write += '.';
            line_to_write += ch;
        } else if (ch == '(' && line_to_write.back() != '\0') {
            line_to_write += '.';
            line_to_write += ch;
        } else if (ch == '\n') {
            return line_to_write;
            // outfile << line_to_write << std::endl;
            // line_to_write = "";  // 清空要写入的字符串
        } else {
            line_to_write += ch;
        }
    }
    return line_to_write;
}

// void nifix_2_postfix() {
//     std::string input_file = "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/regexNifix_jointMark.txt";
//     std::string output_file = "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/regexProcessed.txt";
//
//     std::stack<char> stack_Operator;
//     std::stack<char> stack_Temp;
//     std::string line;
//     char ch;
//
//     std::ifstream file(input_file);
//     std::ofstream outfile(output_file);
//
//     if (!file.is_open()) {
//         std::cerr << "Unable to open file!" << std::endl;
//         return;
//     }
//     while (file.get(ch)) {
//         if(isalpha(ch)) {
//             line += ch;
//         }
//         else if(ch == '(') {
//             stack_Operator.push(ch);
//         }
//         else if(ch == ')') {
//             if(stack_Operator.top() == '(') {std::cout<<"no problem"<<std::endl;}
//             else{std::cerr<<"problem"<<std::endl; return;}
//             stack_Operator.pop();
//         }
//         else if(ch == '\n') {
//             outfile << line << std::endl;
//             line = "";
//         }
//
//     }
//
//     outfile << line << std::endl;
//     line = "";
// }

std::string parseRegex(const std::string &line) {
    std::cout << "parseRegex" << std::endl;
    for (auto ch: line) {
    }
    return "Nothing";
}

void output_NFA_table() {
    tabulate::Table table;
    std::cout << std::endl << "[NFA Table]:" << std::endl;
    table.add_row({"Statue\\symbol", "a", "b", "..."}); // 添加表头
    table.add_row({"1", "{2}", "{3}", "..."}); // 添加表头
    table.add_row({"...", "...", "...", "..."}); // 添加表头

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
