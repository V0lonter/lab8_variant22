#pragma once

#include <optional>
#include <vector>

#include "Database.h"

// Data access component for the dependent entity (no abstract interface is required).
class VoteDao
{
public:
    explicit VoteDao(Storage& storage);

    int add(const Vote& item);
    std::optional<Vote> findById(int id);
    std::vector<Vote> findByCandidate(int candidateId);
    int countByCandidate(int candidateId);
    bool remove(int id);

private:
    Storage& storage_;
};
