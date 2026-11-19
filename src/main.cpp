#include <iostream>

#include "Demos.h"
#include "TestData.h"

int main()
{
    try
    {
        runComparisonDemo();
        runRelationshipDemo();
        runRepositoryDemo();

        std::cout << "\n=== TEST DATA ===\n";
        prepareTestData(kQueryDatabase);

        runQueryDemo();
        runAggregateDemo();
        runJoinDemo();

        runTransactionDemo();
        runRollbackDemo();
        runWithoutTransactionDemo();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Application error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
