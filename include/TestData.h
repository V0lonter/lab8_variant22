#pragma once

#include <string>

// Recreates the database file and fills it with the controlled test data set:
// 5 records of Candidate and 15 records of Vote.
void prepareTestData(const std::string& databaseFile);
