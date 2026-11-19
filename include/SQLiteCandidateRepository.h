#pragma once

#include "Database.h"
#include "ICandidateRepository.h"

class SQLiteCandidateRepository : public ICandidateRepository
{
public:
    explicit SQLiteCandidateRepository(Storage& storage);

    int add(const Candidate& item) override;
    std::optional<Candidate> findById(int id) override;
    std::vector<Candidate> findAll() override;
    bool update(const Candidate& item) override;
    bool remove(int id) override;

private:
    Storage& storage_;
};
