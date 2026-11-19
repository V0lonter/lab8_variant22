#include "TestData.h"

#include <filesystem>
#include <iostream>
#include <vector>

#include "Database.h"

void prepareTestData(const std::string& databaseFile)
{
    // Only the separate demonstration database is removed, so the demo can be re-run.
    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    configureStorage(storage);

    const std::vector<Candidate> items = {
        {0, "Olena Kovalenko", "Unity", "Kyiv", "registered"},
        {0, "Andriy Melnyk", "Unity", "Lviv", "registered"},
        {0, "Iryna Shevchenko", "Greens", "Odesa", "registered"},
        {0, "Taras Bondar", "Civic Union", "Kharkiv", "registered"},
        {0, "Maria Lysenko", "Greens", "Dnipro", "withdrawn"}
    };

    std::vector<int> ids;
    for (const auto& item : items)
    {
        ids.push_back(storage.insert(item));
    }

    const std::vector<Vote> records = {
        {0, ids[0], "2026-11-01", "PS-101", "online", 120},
        {0, ids[0], "2026-11-01", "PS-102", "paper", 85},
        {0, ids[0], "2026-11-02", "PS-101", "online", 240},
        {0, ids[1], "2026-11-01", "PS-201", "paper", 60},
        {0, ids[1], "2026-11-02", "PS-201", "online", 150},
        {0, ids[1], "2026-11-03", "PS-202", "online", 95},
        {0, ids[2], "2026-11-01", "PS-301", "online", 300},
        {0, ids[2], "2026-11-02", "PS-302", "paper", 45},
        {0, ids[2], "2026-11-03", "PS-301", "online", 180},
        {0, ids[3], "2026-11-01", "PS-401", "paper", 75},
        {0, ids[3], "2026-11-02", "PS-401", "paper", 110},
        {0, ids[3], "2026-11-03", "PS-402", "online", 50},
        {0, ids[4], "2026-11-01", "PS-501", "online", 30},
        {0, ids[4], "2026-11-02", "PS-502", "paper", 20},
        {0, ids[4], "2026-11-03", "PS-501", "online", 40}
    };

    for (const auto& record : records)
    {
        storage.insert(record);
    }

    std::cout << "Test database prepared: " << databaseFile << '\n';
    std::cout << "Candidate records: " << storage.count<Candidate>() << '\n';
    std::cout << "Vote records: " << storage.count<Vote>() << '\n';
}
