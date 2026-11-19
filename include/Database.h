#pragma once

#include <stdexcept>
#include <string>

#include <sqlite_orm/sqlite_orm.h>

#include "Candidate.h"
#include "Vote.h"

// ORM mapping: structures -> tables, fields -> columns.
inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;

    return make_storage(
        filename,

        make_table(
            "Candidate",
            make_column("id", &Candidate::id, primary_key().autoincrement()),
            make_column("fullName", &Candidate::fullName, not_null()),
            make_column("party", &Candidate::party, not_null()),
            make_column("region", &Candidate::region, not_null()),
            make_column("status", &Candidate::status, not_null())
        ),

        make_table(
            "Vote",
            make_column("id", &Vote::id, primary_key().autoincrement()),
            make_column("candidateId", &Vote::candidateId, not_null()),
            make_column("castDate", &Vote::castDate, not_null()),
            make_column("pollingStation", &Vote::pollingStation, not_null()),
            make_column("channel", &Vote::channel, not_null()),
            make_column("ballots", &Vote::ballots, not_null()),
            foreign_key(&Vote::candidateId).references(&Candidate::id)
        )
    );
}

using Storage = decltype(createStorage(""));

// Enables foreign key checks for the connection, keeps the connection open
// and synchronizes the schema (creates the tables if they do not exist).
inline void configureStorage(Storage& storage)
{
    storage.on_open = [](sqlite3* db)
    {
        if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            throw std::runtime_error("Cannot enable foreign key constraints");
        }
    };
    storage.open_forever();
    storage.sync_schema();
}
