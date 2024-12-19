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
// #include <unordered_map>
#include <map>
#include <variant>
#include <string.h>
#include "tabulate/table.hpp"
#include "experiment2.h"

StateNode nodes_List[nodeMaxNumber]; // 存储节点
std::stack<StateNode *> travelStack; // 存储遍历过程中的分支，直到遍历到有多父亲节点的、暂时不能编号的节点，才从栈中取出
std::map<StateNode *, int> nodes_visited_times_map; // 遍历中的节点访问次数

int nodes_List_index = 0; // 用于检索 nodes_List
int stateNum = 0; // 节点的编号
// std::vector<Statue_node> statueNodes; // 存储状态机中的各个“节点”结构体
tabulate::Table nfatable;
char nfaList[nodeMaxNumber][3];
int nfaListIndex = 0;


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

    // 如果需要保存为文件
    if (saveAsFile) {
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
        // 关闭文件
        fileWrited.close();
        fileRead.close();
        return;
    }

    // 如果不需要存储文件
    while (std::getline(fileRead, lineRead)) {
        // 逐行读取
        if (lineRead.empty()) continue;
        std::string line_to_write = lineProcessor(lineRead); // 调用传入的函数处理每行
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
                while (true) {
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
    if (op == '*') return 5;
    else if (op == '+')return 4;
    else if (op == '?')return 3;
    else if (op == '.')return 2;
    else if (op == '|')return 1;
    else return 0;
}

std::string parseRegex(const std::string &line) {
    /*
     * 遍历行内的每一个字符，构造自动机，并将代表节点的结构体，存储在一个统一的栈statueStack中
     * 分析过程中暂存中间结果的栈，在本函数域内创建，为联合类型，包括char和Statue_node类型
     */

    // TempResult temp_result{};
    std::stack<std::variant<char, TempResult> > tempStack; // 存储中间变量的栈

    // 遍历行内的每一个字符
    for (char ch: line) {
        // 如果是字母，直接入栈
        if (isalpha(ch)) {
            std::variant<char, TempResult> v = ch;
            tempStack.push(v);
            continue;
        }

        if (ch == '.') {
            operation_conjunction(tempStack);
        } else if (ch == '|') {
            operation_selection(tempStack);
        } else if (ch == '*') {
            operation_closure(tempStack);
        } else if (ch == '+') {
            operation_positiveClosure(tempStack);
        } else if (ch == '?') {
            operation_altinative(tempStack);
        }
    }

    std::cout << "has constructed the automachine" << std::endl;

    // 编号
    // auto head = *std::get<TempResult>(tempStack.top()).head; // 获得状态机的头节点
    // head.stateNum = 0;
    // head.num_of_Daddy = 1; // 手动设置为1，不然无法遍历
    // travel_StateMachine(head, true, false); // 进行编号

    // 输出表 再遍历一次状态机，需要先清空访问次数
    // for (auto &pair: nodes_visited_times_map) {
    //     pair.second = 0; // 将每个值设置为 0
    // }
    // nfatable = tabulate::Table();  // 清空表
    nfatable.add_row({"Statue", "Symbol", "Statue"});

    for (auto node: nodes_List) {
        for (int i = 0; i < node.nextStates.size(); i++) {
            std::cout<<node.stateNum
            << "->"
            <<node.nextStates[i]->stateNum
            << " [label =\" " << node.nextState_conditions[i] << " \" ]"
            <<std::endl;

            std::string symbol;
            symbol.push_back(node.nextState_conditions[i]);
            std::string nextState = std::to_string(node.nextStates[i]->stateNum);
            nfatable.add_row({std::to_string(node.stateNum), symbol, nextState});

            // 加入到List中，为了方便后面遍历
            nfaList[nfaListIndex][0] = char(node.stateNum);
            nfaList[nfaListIndex][1] = node.nextState_conditions[i];
            nfaList[nfaListIndex][2] = char(node.nextStates[i]->stateNum);
            nfaListIndex++;
        }
    }
    // travel_StateMachine(head, false, true);
    output_NFA_table();

    // 为了统一接口
    return "return string to unify the interface";
}


void operation_conjunction(std::stack<std::variant<char, TempResult> > &tempStack) {
    // std::cout << "." << std::endl;
    // 【连接操作】 要取出两个操作对象
    auto object_2 = tempStack.top(); // tempStack取出，放在操作的右边
    tempStack.pop();
    auto object_1 = tempStack.top(); // tempStack取出，放在操作的左边
    tempStack.pop();
    // 判断两个操作对象的类型：用于下面的分支判断
    bool object_1_is_alphabet = std::holds_alternative<char>(object_1);
    bool object_2_is_alphabet = std::holds_alternative<char>(object_2);
    bool object_1_is_struct = !object_1_is_alphabet;
    bool object_2_is_struct = !object_2_is_alphabet;

    // 【连接的对象】都是字符
    if (object_1_is_alphabet and object_2_is_alphabet) {
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];

        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;

        nodes_List_index += 3;

        // 【取出两个对象的值】
        char ch_1 = std::get<char>(object_1);
        char ch_2 = std::get<char>(object_2);
        // std::cout << ch_1 << ch_2 << std::endl;

        // 【2.进行连接操作】
        node_1->nextState_conditions.push_back(ch_1);
        node_1->nextStates.push_back(node_2); // 连接起节点
        node_2->num_of_Daddy++;
        node_2->nextState_conditions.push_back(ch_2);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;

        // 【3.存入结构体数组】
        // nodesList[nodesCount] = node_1;
        // nodesList[nodesCount+1] = node_2;
        // nodesList[nodesCount+2] = node_3;

        // 【4.更新中间结果并入栈】
        TempResult temp_result = {node_1, node_3};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }
    // 【连接的对象】字符 中间结果
    else if (object_1_is_alphabet and object_2_is_struct) {
        StateNode *node_1 = &nodes_List[nodes_List_index];
        node_1->stateNum = nodes_List_index;

        // 【1.取出两个对象的值】
        char ch_1 = std::get<char>(object_1);
        TempResult temp_result = std::get<TempResult>(object_2);

        // 【2.进行连接操作】头变尾不变
        node_1->nextState_conditions.push_back(ch_1);
        node_1->nextStates.push_back(temp_result.head);
        temp_result.head->num_of_Daddy++;

        // 【3.存入结构体数组】
        // nodesList[nodesCount] = node_1;
        nodes_List_index++;

        // 【4.更新中间结果并入栈】
        temp_result.head = node_1;
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }
    // 【连接的对象】中间结果 字符
    else if (object_1_is_struct and object_2_is_alphabet) {
        StateNode *node_2 = &nodes_List[nodes_List_index];
        node_2->stateNum = nodes_List_index;

        // 【1.取出两个对象的值】
        TempResult temp_result = std::get<TempResult>(object_1);
        char ch_2 = std::get<char>(object_2);

        // 【2.进行连接操作】尾变头不变
        temp_result.tail->nextStates.push_back(node_2);
        temp_result.tail->nextState_conditions.push_back(ch_2);
        node_2->num_of_Daddy++;

        // 【3.存入结构体数组】
        // nodesList[nodesCount] = node_2;
        nodes_List_index++;

        // 【4.更新中间结果并入栈】
        temp_result.tail = node_2;
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }
    // 【连接的对象】中间结果 中间结果
    else {
        TempResult temp_result1 = std::get<TempResult>(object_1);
        TempResult temp_result2 = std::get<TempResult>(object_2);

        temp_result1.tail->nextStates.push_back(temp_result2.head);
        temp_result1.tail->nextState_conditions.push_back(emptyState_char);

        temp_result2.head->num_of_Daddy++;

        TempResult temp_result = {temp_result1.head, temp_result2.tail};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }
}

/// 【选择】运算：需要两个操作数
/// @param tempStack
void operation_selection(std::stack<std::variant<char, TempResult> > &tempStack) {
    std::cout << "|" << std::endl;
    // 从栈中取出操作对象
    auto object_2 = tempStack.top(); // tempStack取出，放在操作符的右边
    tempStack.pop();
    auto object_1 = tempStack.top(); // tempStack取出，放在操作符的左边
    tempStack.pop();

    // 判断操作对象的类型
    bool object_1_is_alphabet = std::holds_alternative<char>(object_1);
    bool object_2_is_alphabet = std::holds_alternative<char>(object_2);
    bool object_1_is_struct = !object_1_is_alphabet;
    bool object_2_is_struct = !object_2_is_alphabet;

    // 根据对象类型，重新取得操作对象
    if (object_1_is_alphabet and object_2_is_alphabet) {
        // 【创建需要的数量的节点】
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];
        StateNode *node_4 = &nodes_List[nodes_List_index + 3];
        StateNode *node_5 = &nodes_List[nodes_List_index + 4];
        StateNode *node_6 = &nodes_List[nodes_List_index + 5];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;
        node_4->stateNum = nodes_List_index + 3;
        node_5->stateNum = nodes_List_index + 4;
        node_6->stateNum = nodes_List_index + 5;

        nodes_List_index += 6;

        // 【取出两个对象的值】
        char ch_1 = std::get<char>(object_1);
        char ch_2 = std::get<char>(object_2);

        // 【进行具体操作】
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        node_2->nextState_conditions.push_back(ch_1);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;
        node_3->nextState_conditions.push_back(emptyState_char);
        node_3->nextStates.push_back(node_6);
        node_6->num_of_Daddy++;
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;
        node_4->nextState_conditions.push_back(ch_2);
        node_4->nextStates.push_back(node_5);
        node_5->num_of_Daddy++;
        node_5->nextState_conditions.push_back(emptyState_char);
        node_5->nextStates.push_back(node_6);
        node_6->num_of_Daddy++;

        // 【更新中间结果，并将结果入栈】
        TempResult temp_result = {node_1, node_6};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    } else if (object_1_is_alphabet and object_2_is_struct) {
        // 【取出两个对象的值】
        char ch_1 = std::get<char>(object_1);
        TempResult temp_result = std::get<TempResult>(object_2);

        // 【创建节点】
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];
        StateNode *node_4 = &nodes_List[nodes_List_index + 3];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;
        node_4->stateNum = nodes_List_index + 3;

        nodes_List_index += 4;

        // 【进行具体的运算】
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        node_2->nextState_conditions.push_back(ch_1);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;
        node_3->nextState_conditions.push_back(emptyState_char);
        node_3->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(temp_result.head);
        temp_result.head->num_of_Daddy++;

        temp_result.tail->nextState_conditions.push_back(emptyState_char);
        temp_result.tail->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;

        // 【更新中间结果并入栈】
        temp_result = {node_1, node_4};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    } else if (object_1_is_struct and object_2_is_alphabet) {
        // 【取出两个对象的值】
        TempResult temp_result = std::get<TempResult>(object_1);
        char ch_2 = std::get<char>(object_2);

        // 【创建节点并编号】
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];
        StateNode *node_4 = &nodes_List[nodes_List_index + 3];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;
        node_4->stateNum = nodes_List_index + 3;

        nodes_List_index += 4;

        // 【具体运算】
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        node_2->nextState_conditions.push_back(ch_2);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;
        node_3->nextState_conditions.push_back(emptyState_char);
        node_3->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(temp_result.head);
        temp_result.head->num_of_Daddy++;

        temp_result.tail->nextState_conditions.push_back(emptyState_char);
        temp_result.tail->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;

        // 【更新中间结果并入栈】
        temp_result = {node_1, node_4};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    } else if (object_1_is_struct and object_2_is_struct) {
        // 【创建节点并编号】
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        nodes_List_index += 2;

        // 【取出对象的值】
        TempResult temp_result_1 = std::get<TempResult>(object_1);
        TempResult temp_result_2 = std::get<TempResult>(object_2);

        // 【进行具体的运算】
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(temp_result_1.head);
        temp_result_1.head->num_of_Daddy++;
        temp_result_1.tail->nextState_conditions.push_back(emptyState_char);
        temp_result_1.tail->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(temp_result_2.head);
        temp_result_2.head->num_of_Daddy++;
        temp_result_2.tail->nextState_conditions.push_back(emptyState_char);
        temp_result_2.tail->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;

        // 【更新中间结果并入栈】
        TempResult temp_result = {node_1, node_2};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }
}

void operation_closure(std::stack<std::variant<char, TempResult> > &tempStack) {
    std::cout << "*" << std::endl;
    auto object = tempStack.top(); // tempStack取出
    tempStack.pop();

    bool object_is_alphabet = std::holds_alternative<char>(object);

    if (object_is_alphabet) {
        // 如果取出的是一个字符类型

        // 【创建节点】
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];
        StateNode *node_4 = &nodes_List[nodes_List_index + 3];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;
        node_4->stateNum = nodes_List_index + 3;

        nodes_List_index += 4;


        char ch = std::get<char>(object);
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;

        // node_2 到 node_3 的构建，放在判断语句中
        node_2->nextState_conditions.push_back(ch);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;

        node_3->nextState_conditions.push_back(emptyState_char);
        node_3->nextStates.push_back(node_2);
        node_2->num_of_Daddy++; // 按照原来要遍历状态机的做法，放在这里都不对了，但是现在不遍历了，不管了
        node_3->nextState_conditions.push_back(emptyState_char);
        node_3->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;

        // 更新中间节点并入栈
        TempResult temp_result = {node_1, node_4};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);

    } else {
        // 取出来的是中间结果
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        nodes_List_index += 2;

        TempResult temp_result = std::get<TempResult>(object);

        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(temp_result.head);
        temp_result.head->num_of_Daddy++;
        temp_result.tail->nextState_conditions.push_back(emptyState_char);
        temp_result.tail->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        temp_result.tail->nextState_conditions.push_back(emptyState_char);
        temp_result.tail->nextStates.push_back(temp_result.head);
        node_2->num_of_Daddy++;  // 虽然可能不对，但是先写着
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;

        temp_result = {node_1, node_2};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }

}

void operation_positiveClosure(std::stack<std::variant<char, TempResult> > &tempStack) {
    std::cout << "+" << std::endl;
    auto object = tempStack.top(); // tempStack取出
    tempStack.pop();

    bool object_is_alphabet = std::holds_alternative<char>(object);

    if (object_is_alphabet) {
        // 创建节点
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];
        StateNode *node_4 = &nodes_List[nodes_List_index + 3];
        StateNode *node_5 = &nodes_List[nodes_List_index + 4];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;
        node_4->stateNum = nodes_List_index + 3;
        node_5->stateNum = nodes_List_index + 4;
        nodes_List_index += 5;

        // 取出字符
        char ch = std::get<char>(object);

        // 进行运算
        node_1->nextState_conditions.push_back(ch);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        node_2->nextState_conditions.push_back(emptyState_char);
        node_2->nextStates.push_back(node_5);
        node_5->num_of_Daddy++;
        node_2->nextState_conditions.push_back(emptyState_char);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;
        node_3->nextState_conditions.push_back(ch);
        node_3->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;
        node_4->nextState_conditions.push_back(emptyState_char);
        node_4->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;
        node_4->nextState_conditions.push_back(emptyState_char);
        node_4->nextStates.push_back(node_5);
        node_5->num_of_Daddy++;

        TempResult temp_result = {node_1, node_5};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }else {
        StateNode *node_1 = &nodes_List[nodes_List_index];
        node_1->stateNum = nodes_List_index;
        nodes_List_index += 3;

        TempResult temp_result_mid = std::get<TempResult>(object);
        TempResult temp_result_front = temp_result_mid; // todo: 新多出来的要编号啊，还是涉及到遍历，先不管了

        temp_result_front.tail->nextState_conditions.push_back(emptyState_char);
        temp_result_front.tail->nextStates.push_back(node_1);
        node_1->num_of_Daddy++;

        temp_result_front.tail->nextState_conditions.push_back(emptyState_char);
        temp_result_front.tail->nextStates.push_back(temp_result_mid.head);
        temp_result_mid.head->num_of_Daddy++;

        temp_result_mid.tail->nextState_conditions.push_back(emptyState_char);
        temp_result_mid.tail->nextStates.push_back(node_1);
        node_1->num_of_Daddy++;

        temp_result_mid.tail->nextState_conditions.push_back(emptyState_char);
        temp_result_mid.tail->nextStates.push_back(temp_result_front.head);
        temp_result_front.head->num_of_Daddy++;

        TempResult temp_result = {temp_result_front.head, node_1};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);

        // 进行运算
    }


}

void operation_altinative(std::stack<std::variant<char, TempResult> > &tempStack) {
    std::cout << "?" << std::endl;

    auto object = tempStack.top();
    tempStack.pop();

    bool object_is_alphabet = std::holds_alternative<char>(object);
    if (object_is_alphabet) {
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        StateNode *node_3 = &nodes_List[nodes_List_index + 2];
        StateNode *node_4 = &nodes_List[nodes_List_index + 3];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        node_3->stateNum = nodes_List_index + 2;
        node_4->stateNum = nodes_List_index + 3;
        nodes_List_index += 4;

        char ch = std::get<char>(object);

        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;
        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;
        node_2->nextState_conditions.push_back(ch);
        node_2->nextStates.push_back(node_3);
        node_3->num_of_Daddy++;
        node_3->nextState_conditions.push_back(ch);
        node_3->nextStates.push_back(node_4);
        node_4->num_of_Daddy++;


        TempResult temp_result = {node_1, node_4};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }else {
        StateNode *node_1 = &nodes_List[nodes_List_index];
        StateNode *node_2 = &nodes_List[nodes_List_index + 1];
        node_1->stateNum = nodes_List_index;
        node_2->stateNum = nodes_List_index + 1;
        nodes_List_index += 2;

        TempResult temp_result = std::get<TempResult>(object);

        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;

        node_1->nextState_conditions.push_back(emptyState_char);
        node_1->nextStates.push_back(temp_result.head);
        temp_result.head->num_of_Daddy++;

        temp_result.tail->nextState_conditions.push_back(emptyState_char);
        temp_result.tail->nextStates.push_back(node_2);
        node_2->num_of_Daddy++;

        temp_result = {node_1, node_2};
        std::variant<char, TempResult> result = temp_result;
        tempStack.push(result);
    }

}

// todo:select遍历编号无误

auto travel_StateMachine(StateNode &current, bool number_node_while_travelling, bool write_1row_into_table) -> void {
    // 遍历状态机
    nodes_visited_times_map[&current]++; // 访问次数++

    // 如果该节点被访问的次数 相等于 该节点的父节点数量，说明所有父分支都已经被编号了，此节点可以编号
    bool this_node_canbe_number = nodes_visited_times_map[&current] == current.num_of_Daddy;
    // 该节点有多个子节点
    // bool this_node_has_multi_children = current.nextStates.size() > 1;

    if (!this_node_canbe_number) {
        // 如果本节点尚且不可编号，前面还有一些分支要编号，将他们先从栈中提取出来

        auto another_node = travelStack.top();
        travelStack.pop();
        // 另一个节点将被遍历
        travel_StateMachine(*another_node, number_node_while_travelling, write_1row_into_table);
    } else {
        // 如果是可以完成本节点的遍历的，则函数继续运行，执行遍历中的操作
        if (number_node_while_travelling) {
            // 如果是在编号
            current.stateNum = stateNum;
            if (!current.nextState_conditions.empty())
                std::cout << current.stateNum << '-' << current.nextState_conditions[0] << "->" << std::endl;
            stateNum++;
        }
        if (write_1row_into_table and !current.nextState_conditions.empty()) {
            // nfatable
            std::cout << "write_1row_into_table" << std::endl;
            // todo: 不能说后面一旦没有子节点就马上停下来，因为还可能有多个父分支没有遍历到，不能在这里停止
            for (int i = 0; i < current.nextState_conditions.size(); i++) {
                std::string symbol;
                symbol.push_back(current.nextState_conditions[i]);
                std::string nextState = std::to_string(current.nextStates[i]->stateNum);
                nfatable.add_row({std::to_string(current.stateNum), symbol, nextState});
                std::cout << current.stateNum << '-' << current.nextState_conditions[i] << "->" << current.nextStates[i]
                        ->stateNum << std::endl;
            }
            // for (auto symbol_char: current.nextStatue_conditions) {
            //     std::string symbol;  symbol.push_back(symbol_char);
            //     nfatable.add_row({std::to_string(current.stateNum), symbol, std::to_string(current.nextStates)});
            // }
        }
    }

    // 【递归边界】如果是最后一个节点了，那就返回
    if (current.nextStates.empty()) {
        std::cout << "travel over" << std::endl;
        return;
    }

    // 把有可能的多分支，放入栈中，如果没有多分子，自然不会运行
    for (int i = current.nextStates.size() - 1; i >= 1; i--) {
        // 0号不需要加入栈，因为它接下来就要被访问
        if (current.nextStates[i]->stateNum != -1) {
            // 如果子节点已经被编号过了（为了应对循环的情况）
            continue;
        }
        travelStack.push(current.nextStates[i]);
    }
    travel_StateMachine(*current.nextStates[0], number_node_while_travelling, write_1row_into_table);
}

// get_epsilon_closure()

// void number_node_while_travelling(StateNode &current) {
//     // 遍历过程中给节点编号
//     current.stateNum = stateNum;
//     std::cout<<stateNum<<'-'<<current.nextStatue_conditions[0]<<"->"<<std::endl;
//     stateNum++;
// }

void output_NFA_table() {
    // tabulate::Table table;
    std::cout << std::endl << "[NFA Table]:" << std::endl;
    nfatable.format().border_color(tabulate::Color::yellow).font_color(tabulate::Color::yellow);
    nfatable[0][0].format().font_style({tabulate::FontStyle::bold}); // 为特定单元格设置粗体
    std::cout << nfatable << std::endl;

    std::cout<<"use to verificate"<<std::endl;
    size_t num_rows = nfatable.size();
    size_t num_cols = nfatable[0].size();

    std::cout<<num_cols<<std::endl;
    std::cout<<num_rows<<std::endl;

}
