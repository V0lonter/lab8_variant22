#pragma once

#include <string>

// Main (parent) entity: candidate of the (simplified educational) voting model
struct Candidate
{
    int id = 0;
    std::string fullName;
    std::string party;
    std::string region;
    std::string status;
};
