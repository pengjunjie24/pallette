/*************************************************************************
  > File Name: Sudoku.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月23日 星期日 15时30分29秒
 ************************************************************************/

#ifndef PALLETTE_SUDOKU_H
#define PALLETTE_SUDOKU_H

#include <string>

//数独求解
std::string solveSudoku(const std::string& puzzle);
const int kCells = 81;
extern const char kNoSolution[];

#endif
