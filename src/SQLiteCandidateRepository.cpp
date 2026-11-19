#include "SQLiteCandidateRepository.h"

SQLiteCandidateRepository::SQLiteCandidateRepository(Storage& storage)
    : storage_(storage)
{
}

int SQLiteCandidateRepository::add(const Candidate& item)
{
    return storage_.insert(item);
}

std::optional<Candidate> SQLiteCandidateRepository::findById(int id)
{
    return storage_.get_optional<Candidate>(id);
}

std::vector<Candidate> SQLiteCandidateRepository::findAll()
{
    return storage_.get_all<Candidate>();
}

bool SQLiteCandidateRepository::update(const Candidate& item)
{
    if (!storage_.get_optional<Candidate>(item.id))
    {
        return false;
    }
    storage_.update(item);
    return true;
}

bool SQLiteCandidateRepository::remove(int id)
{
    if (!storage_.get_optional<Candidate>(id))
    {
        return false;
    }
    storage_.remove<Candidate>(id);
    return true;
}
