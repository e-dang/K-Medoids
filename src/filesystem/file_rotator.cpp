#include <fstream>
#include <hpkmediods/filesystem/file_rotator.hpp>

namespace hpkmediods
{
std::string FileRotator::getUniqueFileName(std::string& baseFilepath, std::string identifier)
{
    std::set<char> delims = { '/' };
    std::set<char> dot    = { '.' };

    auto splitFilepath = splitPath(baseFilepath, delims);
    auto splitFileName = splitPath(splitFilepath.back(), dot);

    splitFilepath.pop_back();
    auto ext = splitFileName.back();
    splitFileName.pop_back();
    auto baseFileName = splitFileName.back();
    splitFileName.pop_back();

    attachStringToFileName(baseFileName, identifier);

    return attachFileNum(baseFileName, ext, splitFilepath);
}

std::vector<std::string> FileRotator::splitPath(const std::string& str, const std::set<char> delimiters)
{
    std::vector<std::string> result;

    char const* pch   = str.c_str();
    char const* start = pch;
    for (; *pch; ++pch)
    {
        if (delimiters.find(*pch) != delimiters.end())
        {
            if (start != pch)
            {
                std::string str(start, pch);
                result.push_back(str);
            }
            else
            {
                result.push_back("");
            }
            start = pch + 1;
        }
    }

    result.push_back(start);

    return result;
}
void FileRotator::attachStringToFileName(std::string& baseFileName, std::string& identifier)
{
    baseFileName += "_" + identifier;
}

std::string FileRotator::attachFileNum(const std::string& baseFileName, std::string& ext,
                                       std::vector<std::string>& splitFilepath)
{
    int count = 0;
    while (true)
    {
        auto newFileName = baseFileName + "_" + std::to_string(count);
        newFileName      = reassemblePath(newFileName, ext, splitFilepath);
        std::ifstream file(newFileName.c_str());
        if (file.fail())
        {
            return newFileName;
        }

        count += 1;
    }
}

std::string FileRotator::reassemblePath(std::string& newFileName, std::string& ext,
                                        std::vector<std::string>& splitFilepath)
{
    std::string newFilePath = "";
    for (const auto& val : splitFilepath)
    {
        newFilePath += val + "/";
    }
    newFilePath += newFileName + "." + ext;

    return newFilePath;
}
}  // namespace hpkmediods