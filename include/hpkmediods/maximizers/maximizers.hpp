#pragma once

#include <hpkmediods/maximizers/pam_swap.hpp>
#include <memory>
#include <string>

namespace hpkmediods
{
template <typename T, Parallelism Level>
std::unique_ptr<IMaximizer<T>> createMaximizer(const std::string& maximizerString)
{
    if (maximizerString == PAM)
        return std::make_unique<PAMSwap<T, Level>>();
    else
        std::cerr << "Unrecognized maximizer string!\n";

    exit(1);
}
}  // namespace hpkmediods