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

void processFilebyLine(
    bool saveAsFile, const std::string &filePath_input,
    const std::string &filePath_output, const std::function<std::string(const std::string &)> &lineProcessor) {
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

std::string preprocessing_addJointMark(const std::string &line) {
    // 在需要的地方，添加连接符号

    std::string line_to_return;

    for (char ch: line) {
        if (isalpha(ch)
            && line_to_return.back() != '('
            && line_to_return.back() != '|'
            && line_to_return.back() != '/'
            && line_to_return.back() != '\0') {
            line_to_return += '.';
            line_to_return += ch;
        } else if (ch == '(' && line_to_return.back() != '\0') {
            line_to_return += '.';
            line_to_return += ch;
        } else if (ch == '\n') {
            return line_to_return;
            // outfile << line_to_write << std::endl;
            // line_to_write = "";  // 清空要写入的字符串
        } else {
            line_to_return += ch;
        }
    }
    return line_to_return;
}


std::string preprocessing_nifix_2_postfix(const std::string &line) {
    std::string line_to_return;
    std::stack<char> postfixStack;

    // 遍历表达式的每一个字符
    for (char ch: line) {
        // 如果是个字母
        if (isalpha(ch)) { line_to_return += ch; }

        // 如果是操作符
        else {
            // 如果栈顶元素为 '('，操作符可直接入栈
            // todo: 是否有必要判断!postfixStack.empty()
            if (!postfixStack.empty() && postfixStack.top() == '(') {
                postfixStack.push(ch);
                continue;
            }

            // 判断操作符的类型
            // 如果为左括号，直接入栈
            if (ch == '(') {
                postfixStack.push(ch);
            }
            // 如果为右括号，一直弹栈，添加到 line_to_return中，直到弹出一个‘(’
            else if (ch == ')') {
                while (1) {
                    // 遇到左括号，直接弹出，并终止掉此循环
                    if (postfixStack.top() == '(') {
                        postfixStack.pop();
                        break;
                    }
                    line_to_return += postfixStack.top();
                    postfixStack.pop();
                }
            } else {
                // 普通的操作符要比较优先级
                // 如果栈中什么都没有，就把普通字符直接放进去
                if (postfixStack.empty()) {
                    postfixStack.push(ch);
                    continue;
                }

                // 如果栈中有其它普通符号，才进行比较
                int priority_StackTop = getRegexOpPriority(postfixStack.top());
                int priority_ch = getRegexOpPriority(ch);

                // 要入栈的的优先级 <= 要出栈的
                if (priority_ch <= priority_StackTop) {
                    // 先出栈并加入line_to_return，再入栈
                    line_to_return += postfixStack.top(); // 加
                    postfixStack.pop(); // 出栈
                    postfixStack.push(ch); // 入栈
                }
                // 要入栈的优先级，高于要出栈的
                else {
                    // 直接入栈
                    postfixStack.push(ch);
                }
            }
        }
    }

    // 最后要把所有栈输出
    while (!postfixStack.empty()) {
        line_to_return += postfixStack.top();
        postfixStack.pop();
    }
    // 循环结束，返回后缀表达式
    return line_to_return;
}

int getRegexOpPriority(char op) {
    // 获取 符号的优先级
    switch (op) {
        case '*': return 5;
        case '+': return 4;
        case '?': return 3;
        case '.': return 2;
        case '|': return 1;
        default: ;
    }
}

// std::string parseRegex(const std::string &lineRead) {
//     const int lineLen = lineRead.length();
//     int statue = 0; // todo: statue在下面可能要考虑从栈中取得
//     int i, j = 0;
//     tabulate::Table tableNFA;
//     tableNFA.add_row({"statue", "symbole", "statue"});
//     tableNFA.format().border_color(tabulate::Color::yellow).font_color(tabulate::Color::yellow);
//
//     for (i = 0; i < lineLen - 1; i++) {
//         // 确定ch_now和ch_ahead
//         j = i + 1;
//         char ch_now = lineRead[i];
//         char ch_ahead = lineRead[j];
//         std::cout << ch_now << ch_ahead << std::endl;
//
//         // 进行解析【核心】
//         if ((ch_now == '?' or ch_now == '*' or ch_now == '+') and (
//                 std::isalpha(ch_ahead))) {
//             std::cout << "conjunction" << std::endl << std::endl;
//         } else if (std::isalpha(ch_now)) {
//             // 如果ch_now是个字母，再进一步判断后面有什么
//             if (ch_ahead == '.') {
//                 // 必然是后面连接着字母，不可能是符号了
//                 // std::cout << "add statue" << std::endl;
//                 // std::cout << statue << "-" << ch_now << "->" << statue+1 << std::endl;
//                 write_1row_into_table(statue, ch_now, statue + 1, tableNFA);
//                 statue++;
//             } else if (ch_ahead == '*') {
//                 std::cout << "closure" << std::endl << std::endl;
//                 // 简单版本实现
//                 write_1row_into_table(statue, '@', statue + 1, tableNFA);
//                 write_1row_into_table(statue + 1, ch_now, statue + 2, tableNFA);
//                 write_1row_into_table(statue + 2, '@', statue + 1, tableNFA);
//                 write_1row_into_table(statue + 2, '@', statue + 3, tableNFA);
//                 write_1row_into_table(statue, '@', statue + 3, tableNFA);
//                 statue += 3;
//             } else if (ch_ahead == '+') {
//                 std::cout << "positive closure" << std::endl << std::endl;
//             } else if (ch_ahead == '?') {
//                 std::cout << "alternative" << std::endl << std::endl;
//             } else if (ch_ahead == '|') {
//                 std::cout << "select" << std::endl << std::endl;
//                 write_1row_into_table(statue, '@', statue + 1, tableNFA);
//                 write_1row_into_table(statue + 1, ch_now, statue + 2, tableNFA);
//                 write_1row_into_table(statue + 2, '@', statue + 5, tableNFA);
//                 write_1row_into_table(statue, '@', statue + 3, tableNFA);
//                 // todo: 如果是嵌套，比如(a|b*)这种直接相加的做法应该要出问题了，不知道怎么做了
//             }
//         } else if (ch_now == '.') {
//             if (ch_ahead == '(') {
//                 std::cout << "next one is select" << std::endl << std::endl;
//             } else if (std::isalpha(ch_ahead) and j == lineLen - 1) {
//                 // 连接的字母是最后一个字母
//                 // std::cout << "the last alphabet" << std::endl << std::endl;
//                 write_1row_into_table(statue, ch_ahead, statue + 1, tableNFA);
//             }
//         }
//         std::cout << std::endl;
//     }
//     std::cout << "---------------------" << std::endl;
//     std::cout << tableNFA << std::endl;
//     // 重置状态
//     statue = 0;
//     return "Nothing";
// }
//
// void write_1row_into_table(int statue1, char symbol, int statue2, tabulate::Table table) {
//     table.add_row({
//         std::to_string(statue1),
//         std::string(1, symbol),
//         std::to_string(statue2)
//     });
// }


// void output_NFA_table() {
//     tabulate::Table table;
//     std::cout << std::endl << "[NFA Table]:" << std::endl;
//     table.add_row({"Statue\\symbol", "a", "b", "..."}); // 添加表头
//     table.add_row({"1", "{2}", "{3}", "..."}); // 添加表头
//     table.add_row({"...", "...", "...", "..."}); // 添加表头
// for (auto & i : node) {
//     // if (i.symbol[1] == '\0') break;  // symbol中没有任何内容，就不要输入了
//     table.add_row({
//         std::to_string(i.statue_pre[0]),
//         std::string(1, i.symbol[0]),
//         std::to_string(i.statue_next[0])
//     });
// }
// table.format().border_color(tabulate::Color::yellow).font_color(tabulate::Color::yellow);
// table[0][0].format().font_style({tabulate::FontStyle::bold}); // 为特定单元格设置粗体
//     std::cout << table << std::endl;
// }
