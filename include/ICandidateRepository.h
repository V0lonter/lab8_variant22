#pragma once

#include <optional>
#include <vector>

#include "Candidate.h"

// Repository interface: contains no SQLite or sqlite_orm types.
class ICandidateRepository
{
public:
    virtual ~ICandidateRepository() = default;

    // Create
    virtual int add(const Candidate& item) = 0;

    // Read
    virtual std::optional<Candidate> findById(int id) = 0;
    virtual std::vector<Candidate> findAll() = 0;

    // Update: false means that the record does not exist
    virtual bool update(const Candidate& item) = 0;

    // Delete: false means that the record does not exist
    virtual bool remove(int id) = 0;
};
