#pragma once

#include <hpkmedoids/initializers/pam_build.hpp>
#include <hpkmedoids/initializers/random_initializer.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace hpkmedoids
{
template <typename T, Parallelism Level>
std::unique_ptr<IInitializer<T>> createInitializer(const std::string& initializerString)
{
    if (initializerString == RANDOM_INIT)
        return std::make_unique<RandomInitializer<T>>();
    else if (initializerString == PAM_INIT)
        return std::make_unique<PAMBuild<T, Level>>();
    else
        std::cerr << "Unrecognized initializer string!\n";

    exit(1);
}
}  // namespace hpkmedoids