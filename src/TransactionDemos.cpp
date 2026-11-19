#include <filesystem>
#include <iostream>

#include "Demos.h"
#include "Database.h"

namespace
{
void printCounts(Storage& storage, const char* title)
{
    std::cout << '\n' << title << ":\n";
    std::cout << "Candidate: " << storage.count<Candidate>() << '\n';
    std::cout << "Vote: " << storage.count<Vote>() << '\n';
}

bool exists(Storage& storage, const std::string& name)
{
    using namespace sqlite_orm;
    return !storage.get_all<Candidate>(where(c(&Candidate::fullName) == name)).empty();
}
}

// Registration of a candidate together with the first record of votes (two related operations).
void runTransactionDemo()
{
    auto storage = createStorage(kQueryDatabase);
    configureStorage(storage);

    std::cout << "\n=== 7. TRANSACTION: COMMIT ===\n";
    printCounts(storage, "Before transaction");

    bool committed = storage.transaction([&]()
    {
        Candidate item{0, "Petro Hrytsenko", "Unity", "Poltava", "registered"};
        int parentId = storage.insert(item);
        std::cout << "\nCandidate inserted inside transaction, ID: " << parentId << '\n';

        Vote record{0, parentId, "2026-11-04", "PS-601", "online", 70};
        storage.insert(record);
        std::cout << "Vote inserted inside transaction\n";

        return true;   // COMMIT
    });

    std::cout << "\nTransaction committed: " << std::boolalpha << committed << '\n';
    printCounts(storage, "After transaction");
    std::cout << "Candidate 'Petro Hrytsenko' exists: " << exists(storage, "Petro Hrytsenko") << '\n';
}

void runRollbackDemo()
{
    auto storage = createStorage(kQueryDatabase);
    configureStorage(storage);

    std::cout << "\n=== 8. TRANSACTION: ROLLBACK ===\n";
    printCounts(storage, "Before transaction");

    try
    {
        storage.transaction([&]()
        {
            Candidate item{0, "Dmytro Sydorenko", "Civic Union", "Sumy", "registered"};
            int parentId = storage.insert(item);
            std::cout << "\nCandidate inserted inside transaction, ID: " << parentId << '\n';

            // The second operation is incorrect on purpose: the foreign key refers to a missing record.
            Vote record{0, -1, "2026-11-04", "PS-701", "paper", 55};
            storage.insert(record);

            return true;
        });
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nTransaction failed: " << ex.what() << '\n';
        std::cout << "Changes rolled back\n";
    }

    printCounts(storage, "After transaction");
    std::cout << "Candidate 'Dmytro Sydorenko' exists: " << std::boolalpha
              << exists(storage, "Dmytro Sydorenko") << '\n';

    // The function of the transaction may also request a rollback by returning false.
    bool committed = storage.transaction([&]()
    {
        storage.insert(Candidate{0, "Anna Moroz", "Greens", "Rivne", "registered"});
        return false;   // ROLLBACK
    });
    std::cout << "\nTransaction returned false, committed: " << std::boolalpha << committed << '\n';
    std::cout << "Candidate 'Anna Moroz' exists: " << exists(storage, "Anna Moroz") << '\n';
    printCounts(storage, "After rollback requested by the function");
}

void runWithoutTransactionDemo()
{
    const std::string file = "voting_no_transaction.db";
    std::filesystem::remove(file);

    auto storage = createStorage(file);
    configureStorage(storage);

    std::cout << "\n=== 9. THE SAME OPERATIONS WITHOUT A TRANSACTION ===\n";
    printCounts(storage, "Before operations");

    try
    {
        Candidate item{0, "Viktor Kravchuk", "Greens", "Chernihiv", "registered"};
        int parentId = storage.insert(item);
        std::cout << "\nCandidate inserted, ID: " << parentId << '\n';

        Vote record{0, -1, "2026-11-04", "PS-801", "online", 65};
        storage.insert(record);
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nOperation failed: " << ex.what() << '\n';
    }

    printCounts(storage, "After operations");
    std::cout << "Candidate 'Viktor Kravchuk' exists: " << std::boolalpha
              << exists(storage, "Viktor Kravchuk") << " (the first operation was not rolled back)\n";
}
