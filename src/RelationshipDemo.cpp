#include <filesystem>
#include <iostream>

#include "Demos.h"
#include "Database.h"
#include "VoteDao.h"
#include "SQLiteCandidateRepository.h"

void runRelationshipDemo()
{
    const std::string file = "voting_relation.db";
    std::filesystem::remove(file);

    auto storage = createStorage(file);
    configureStorage(storage);

    std::cout << "\n=== 2. ORM MAPPING AND ONE-TO-MANY RELATIONSHIP ===\n";

    std::cout << "\nTables created by sync_schema():\n";
    for (const char* name : {"Candidate", "Vote"})
    {
        std::cout << name << " exists: " << std::boolalpha << storage.table_exists(name) << '\n';
        for (const auto& column : storage.pragma.table_info(name))
        {
            std::cout << "  " << column.name << " " << column.type
                      << (column.pk ? " PRIMARY KEY" : "") << '\n';
        }
    }

    SQLiteCandidateRepository repository(storage);
    VoteDao dao(storage);

    int parentId = repository.add(Candidate{0, "Demo Candidate", "Demo Party", "Kyiv", "registered"});
    std::cout << "\nCandidate ID: " << parentId << '\n';

    dao.add(Vote{0, parentId, "2026-11-01", "PS-901", "online", 25});
    dao.add(Vote{0, parentId, "2026-11-02", "PS-902", "paper", 40});

    std::cout << "\nVote records of Candidate " << parentId << ":\n";
    for (const auto& record : dao.findByCandidate(parentId))
    {
        std::cout << record.id << " | " << record.candidateId << " | " << record.castDate << " | "
                  << record.pollingStation << " | " << record.channel << " | " << record.ballots << '\n';
    }

    // Referential integrity test: the parent with ID -1 does not exist.
    try
    {
        dao.add(Vote{0, -1, "2026-11-03", "PS-999", "online", 10});
        std::cout << "\nUnexpected insertion success\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nForeign key constraint test:\n";
        std::cout << "Insertion rejected: " << ex.what() << '\n';
    }

    std::cout << "Vote records after the rejected insertion: " << storage.count<Vote>() << '\n';
}
