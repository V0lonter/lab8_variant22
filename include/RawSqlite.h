#pragma once

#include <optional>
#include <string>

#include <sqlite3.h>

#include "Candidate.h"

// RAII wrapper of a SQLite connection (SQLite C API).
class RawDatabase
{
public:
    explicit RawDatabase(const std::string& filename);
    ~RawDatabase();

    RawDatabase(const RawDatabase&) = delete;
    RawDatabase& operator=(const RawDatabase&) = delete;

    sqlite3* get() const { return db_; }

private:
    sqlite3* db_ = nullptr;
};

// The same operations that are implemented through sqlite_orm in the ORM part.
void createRawTable(RawDatabase& database);
int insertCandidateRaw(RawDatabase& database, const Candidate& item);
std::optional<Candidate> findCandidateRawById(RawDatabase& database, int id);
