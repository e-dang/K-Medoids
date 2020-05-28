#pragma once

#include <hpkmediods/initializers/pam_build.hpp>
#include <hpkmediods/initializers/random_initializer.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace hpkmediods
{
template <typename T, Parallelism Level>
std::shared_ptr<IInitializer<T>> createInitializer(const std::string& initializerString)
{
    if (initializerString == RANDOM_INIT)
        return std::make_shared<RandomInitializer<T>>();
    else if (initializerString == PAM_INIT)
        return std::make_shared<PAMBuild<T, Level>>();
    else
        std::cerr << "Unrecognized initializer string!\n";

    exit(1);
}
}  // namespace hpkmediods