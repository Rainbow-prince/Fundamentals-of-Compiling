//
// Created by A123 on 12/5/2024.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cctype>  // 字符处理
#include "experiment2.h"

int main() {
    // 预处理
    const std::string rawfile =
            "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/rawRegexNifix.txt";
    const std::string fileProcessed =
            "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/regexProcessed.txt";

    processFilebyLine(true, rawfile, fileProcessed, addJointMark); // 添加连接符号
    processFilebyLine(false, fileProcessed, fileProcessed, parseRegex); // 添加连接符号

    return 0;
}
