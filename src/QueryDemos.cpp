#include <iostream>

#include "Demos.h"
#include "Database.h"
#include "ValueUtils.h"

namespace
{
void printParent(const Candidate& item)
{
    std::cout << item.id << " | " << item.fullName << " | " << item.party << " | "
              << item.region << " | " << item.status << '\n';
}

void printChild(const Vote& record)
{
    std::cout << record.id << " | " << record.candidateId << " | " << record.castDate << " | "
              << record.pollingStation << " | " << record.channel << " | " << record.ballots << '\n';
}

Storage openQueryStorage()
{
    auto storage = createStorage(kQueryDatabase);
    configureStorage(storage);
    return storage;
}
}

void runQueryDemo()
{
    using namespace sqlite_orm;

    auto storage = openQueryStorage();

    std::cout << "\n=== 4. QUERIES: FILTERING, SORTING, LIMIT ===\n";

    std::cout << "\n4.1 Candidate: status = 'registered':\n";
    auto filtered = storage.get_all<Candidate>(
        where(c(&Candidate::status) == "registered"));
    for (const auto& item : filtered)
    {
        printParent(item);
    }

    std::cout << "\n4.2 Vote: channel = 'online' AND ballots >= 100:\n";
    auto filteredRecords = storage.get_all<Vote>(
        where(c(&Vote::channel) == "online" and c(&Vote::ballots) >= 100),
        order_by(&Vote::id));
    for (const auto& record : filteredRecords)
    {
        printChild(record);
    }

    std::cout << "\n4.3 Candidate sorted by fullName:\n";
    auto sorted = storage.get_all<Candidate>(order_by(&Candidate::fullName).asc());
    for (const auto& item : sorted)
    {
        printParent(item);
    }

    std::cout << "\n4.4 Top 3 Vote records by ballots:\n";
    auto top = storage.get_all<Vote>(order_by(&Vote::ballots).desc(), limit(3));
    for (const auto& record : top)
    {
        printChild(record);
    }

    std::cout << "\n4.5 Candidate: party = 'Unity', sorted by fullName:\n";
    auto byKind = storage.get_all<Candidate>(
        where(c(&Candidate::party) == "Unity"),
        order_by(&Candidate::fullName).asc());
    for (const auto& item : byKind)
    {
        printParent(item);
    }
}

void runAggregateDemo()
{
    using namespace sqlite_orm;

    auto storage = openQueryStorage();

    std::cout << "\n=== 5. AGGREGATE QUERIES ===\n";

    std::cout << "\n5.1 COUNT: " << storage.count<Vote>() << '\n';
    std::cout << "5.2 SUM(ballots): " << asDouble(storage.sum(&Vote::ballots)) << '\n';
    std::cout << "5.3 AVG(ballots): " << asDouble(storage.avg(&Vote::ballots)) << '\n';
    std::cout << "5.4 MAX(ballots): " << asDouble(storage.max(&Vote::ballots)) << '\n';

    std::cout << "\n5.5 GROUP BY candidateId: records and SUM(ballots):\n";
    auto groups = storage.select(
        columns(&Vote::candidateId, count(&Vote::id), sum(&Vote::ballots)),
        group_by(&Vote::candidateId),
        order_by(&Vote::candidateId));
    for (const auto& [parentId, records, total] : groups)
    {
        std::cout << "Candidate ID: " << parentId << " | records: " << records
                  << " | total ballots: " << asDouble(total) << '\n';
    }

    std::cout << "\n5.6 GROUP BY channel: records and AVG(ballots):\n";
    auto byChannel = storage.select(
        columns(&Vote::channel, count(&Vote::id), avg(&Vote::ballots)),
        group_by(&Vote::channel),
        order_by(&Vote::channel));
    for (const auto& [name, records, average] : byChannel)
    {
        std::cout << name << " | records: " << records
                  << " | average ballots: " << asDouble(average) << '\n';
    }
}

void runJoinDemo()
{
    using namespace sqlite_orm;

    auto storage = openQueryStorage();

    std::cout << "\n=== 6. JOIN QUERIES ===\n";

    std::cout << "\n6.1 Candidate + Vote (INNER JOIN):\n";
    auto rows = storage.select(
        columns(&Candidate::fullName, &Vote::castDate, &Vote::pollingStation, &Vote::ballots),
        inner_join<Vote>(on(c(&Candidate::id) == &Vote::candidateId)),
        order_by(&Vote::id));
    for (const auto& [name, date, text, number] : rows)
    {
        std::cout << name << " | " << date << " | " << text << " | " << number << '\n';
    }

    std::cout << "\n6.2 INNER JOIN with filter ballots > 100:\n";
    auto filtered = storage.select(
        columns(&Candidate::fullName, &Vote::pollingStation, &Vote::ballots),
        inner_join<Vote>(on(c(&Candidate::id) == &Vote::candidateId)),
        where(c(&Vote::ballots) > 100),
        order_by(&Vote::ballots).desc());
    for (const auto& [name, text, number] : filtered)
    {
        std::cout << name << " | " << text << " | " << number << '\n';
    }

    std::cout << "\n6.3 Total ballots of every Candidate (JOIN + GROUP BY):\n";
    auto totals = storage.select(
        columns(&Candidate::fullName, sum(&Vote::ballots)),
        inner_join<Vote>(on(c(&Candidate::id) == &Vote::candidateId)),
        group_by(&Candidate::id),
        order_by(&Candidate::id));
    for (const auto& [name, total] : totals)
    {
        std::cout << name << " | " << asDouble(total) << '\n';
    }
}
