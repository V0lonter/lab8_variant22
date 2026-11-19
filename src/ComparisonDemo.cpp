#include <filesystem>
#include <iostream>

#include "Demos.h"
#include "Database.h"
#include "RawSqlite.h"

void runComparisonDemo()
{
    const std::string file = "voting_compare.db";
    std::filesystem::remove(file);

    std::cout << "\n=== 1. SQLITE C API vs sqlite_orm ===\n";

    Candidate item{0, "Oksana Tkachenko", "Unity", "Vinnytsia", "registered"};

    // --- SQLite C API: prepared statements, manual binding and conversion ---
    std::cout << "\n[SQLite C API]\n";
    {
        RawDatabase database(file);
        createRawTable(database);

        int id = insertCandidateRaw(database, item);
        std::cout << "Inserted ID: " << id << '\n';

        auto found = findCandidateRawById(database, id);
        if (found)
        {
            std::cout << "Found: " << found->id << " | " << found->fullName << " | "
                      << found->party << " | " << found->region << " | " << found->status << '\n';
        }
    }

    // --- sqlite_orm: the same operations ---
    std::cout << "\n[sqlite_orm]\n";
    {
        auto storage = createStorage(file);
        configureStorage(storage);

        int id = storage.insert(item);
        std::cout << "Inserted ID: " << id << '\n';

        auto found = storage.get_optional<Candidate>(id);
        if (found)
        {
            std::cout << "Found: " << found->id << " | " << found->fullName << " | "
                      << found->party << " | " << found->region << " | " << found->status << '\n';
        }
    }
}
