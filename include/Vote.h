#pragma once

#include <string>

// Dependent (child) entity: record of votes cast for a candidate at a polling station
// candidateId is a foreign key that refers to Candidate::id.
struct Vote
{
    int id = 0;
    int candidateId = 0;
    std::string castDate;
    std::string pollingStation;
    std::string channel;
    int ballots = 0;
};
