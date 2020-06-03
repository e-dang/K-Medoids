#pragma once

#include <set>
#include <string>
#include <vector>

namespace hpkmediods
{
class FileRotator
{
public:
    FileRotator() {}

    ~FileRotator() {}

    std::string getUniqueFileName(std::string& baseFilepath, std::string identifier);

private:
    void attachStringToFileName(std::string& filepath, std::string& identifier);

    std::string getBaseFileName(std::string& filepath);

    // https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
    std::vector<std::string> splitPath(const std::string& str, const std::set<char> delimiters);

    std::string attachFileNum(const std::string& filepath, std::string& ext, std::vector<std::string>& splitFilepath);

    std::string reassemblePath(std::string& newFileName, std::string& ext, std::vector<std::string>& splitFilepath);
};
}  // namespace hpkmediods