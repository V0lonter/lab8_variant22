#include <filesystem>
#include <iostream>

#include "Demos.h"
#include "Database.h"
#include "SQLiteCandidateRepository.h"

namespace
{
void print(const Candidate& item)
{
    std::cout << item.id << " | " << item.fullName << " | " << item.party << " | "
              << item.region << " | " << item.status << '\n';
}
}

void runRepositoryDemo()
{
    const std::string file = "voting_repository.db";
    std::filesystem::remove(file);

    auto storage = createStorage(file);
    configureStorage(storage);

    SQLiteCandidateRepository concrete(storage);
    ICandidateRepository& repository = concrete;   // work through the interface

    std::cout << "\n=== 3. REPOSITORY (CRUD) ===\n";

    std::cout << "\nCREATE\n";
    int id = repository.add(Candidate{0, "Test Candidate", "Independent", "Zhytomyr", "registered"});
    std::cout << "Inserted ID: " << id << '\n';

    std::cout << "\nREAD\n";
    auto found = repository.findById(id);
    if (found)
    {
        print(*found);
    }

    std::cout << "\nUPDATE\n";
    if (found)
    {
        found->status = "withdrawn";
        std::cout << "Updated: " << std::boolalpha << repository.update(*found) << '\n';
        auto after = repository.findById(id);
        if (after)
        {
            std::cout << "New status: " << after->status << '\n';
        }
    }

    std::cout << "\nREAD ALL\n";
    for (const auto& item : repository.findAll())
    {
        print(item);
    }

    std::cout << "\nDELETE\n";
    std::cout << "Removed: " << std::boolalpha << repository.remove(id) << '\n';
    std::cout << "Record exists after deletion: " << repository.findById(id).has_value() << '\n';

    std::cout << "\nMISSING RECORD (ID 9999)\n";
    std::cout << "findById: " << (repository.findById(9999) ? "found" : "not found") << '\n';
    Candidate missing{9999, "Test Candidate", "Independent", "Zhytomyr", "registered"};
    std::cout << "update: " << std::boolalpha << repository.update(missing) << '\n';
    std::cout << "remove: " << std::boolalpha << repository.remove(9999) << '\n';
}
