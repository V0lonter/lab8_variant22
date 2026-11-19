#include "RawSqlite.h"

#include <memory>
#include <stdexcept>

namespace
{
using StatementPtr = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

StatementPtr prepare(sqlite3* db, const char* sql)
{
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    return StatementPtr(stmt, &sqlite3_finalize);
}

void bindText(sqlite3* db, sqlite3_stmt* stmt, int index, const std::string& value)
{
    if (sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
}

std::string columnText(sqlite3_stmt* stmt, int index)
{
    const unsigned char* text = sqlite3_column_text(stmt, index);
    return text ? reinterpret_cast<const char*>(text) : "";
}
}

RawDatabase::RawDatabase(const std::string& filename)
{
    if (sqlite3_open(filename.c_str(), &db_) != SQLITE_OK)
    {
        std::string message = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        throw std::runtime_error("Cannot open database: " + message);
    }
}

RawDatabase::~RawDatabase()
{
    sqlite3_close(db_);
}

void createRawTable(RawDatabase& database)
{
    char* errorMessage = nullptr;
    int rc = sqlite3_exec(database.get(), R"(
        CREATE TABLE IF NOT EXISTS Candidate (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            fullName TEXT NOT NULL,
            party TEXT NOT NULL,
            region TEXT NOT NULL,
            status TEXT NOT NULL
        );)", nullptr, nullptr, &errorMessage);
    if (rc != SQLITE_OK)
    {
        std::string message = errorMessage ? errorMessage : "Unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error(message);
    }
}

int insertCandidateRaw(RawDatabase& database, const Candidate& item)
{
    sqlite3* db = database.get();
    auto stmt = prepare(db, R"(
        INSERT INTO Candidate (fullName, party, region, status)
        VALUES (?, ?, ?, ?);)");

    bindText(db, stmt.get(), 1, item.fullName);
    bindText(db, stmt.get(), 2, item.party);
    bindText(db, stmt.get(), 3, item.region);
    bindText(db, stmt.get(), 4, item.status);

    if (sqlite3_step(stmt.get()) != SQLITE_DONE)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

std::optional<Candidate> findCandidateRawById(RawDatabase& database, int id)
{
    sqlite3* db = database.get();
    auto stmt = prepare(db, R"(
        SELECT id, fullName, party, region, status
        FROM Candidate
        WHERE id = ?;)");

    if (sqlite3_bind_int(stmt.get(), 1, id) != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    int rc = sqlite3_step(stmt.get());
    if (rc == SQLITE_ROW)
    {
        Candidate item;
        item.id = sqlite3_column_int(stmt.get(), 0);
        item.fullName = columnText(stmt.get(), 1);
        item.party = columnText(stmt.get(), 2);
        item.region = columnText(stmt.get(), 3);
        item.status = columnText(stmt.get(), 4);
        return item;
    }
    if (rc != SQLITE_DONE)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    return std::nullopt;
}
