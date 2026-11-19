#pragma once

#include <string>

// Database file used by the query and transaction demonstrations.
inline const std::string kQueryDatabase = "voting_queries.db";

// Task 2: the same operations through SQLite C API and through sqlite_orm.
void runComparisonDemo();

// Task 3: ORM mapping, foreign key, initial data.
void runRelationshipDemo();

// Task 4: CRUD through the repository interface.
void runRepositoryDemo();

// Task 5: filtering, sorting, aggregation, GROUP BY, JOIN (uses kQueryDatabase).
void runQueryDemo();
void runAggregateDemo();
void runJoinDemo();

// Task 6: transactions (use kQueryDatabase and separate files).
void runTransactionDemo();
void runRollbackDemo();
void runWithoutTransactionDemo();
