//
// Created by A123 on 12/5/2024.
//
#include <filesystem>
#include <string>
#include "experiment2.h"

int main() {
    // 预处理
    const std::string rawfile =
            "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/rawRegexNifix.txt";
    const std::string file_mid =
            "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/mid.txt";
    const std::string fileProcessed =
            "E:/100-BUSSINESS_MAJOR/Project_Clion/Fundemental principal of compile/Fundamentals-of-Compiling/Write_My_Compiler/experiment2/regexProcessed.txt";

    processFilebyLine(true, rawfile, file_mid, preprocessing_addJointMark); // 添加连接符号
    processFilebyLine(true, file_mid, fileProcessed, preprocessing_nifix_2_postfix);
    processFilebyLine(false, fileProcessed, fileProcessed, Regex2NFA); // 添加连接符号
    return 0;
}
