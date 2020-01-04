#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <filesystem>
#elif defined __linux__
#include <experimental/filesystem>
#endif

namespace filesys = std::experimental::filesystem;

/*
* Get the list of all files in given directory and its sub directories.
*
* Arguments
* 	dirPath : Path of directory to be traversed
*
* Returns:
* 	vector containing paths of all the files in given directory and its sub directories
*
*/
std::vector<std::string> GetAllFilesInDir(const std::string &dirPath);
