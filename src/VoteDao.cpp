#include "VoteDao.h"

VoteDao::VoteDao(Storage& storage)
    : storage_(storage)
{
}

int VoteDao::add(const Vote& item)
{
    return storage_.insert(item);
}

std::optional<Vote> VoteDao::findById(int id)
{
    return storage_.get_optional<Vote>(id);
}

std::vector<Vote> VoteDao::findByCandidate(int candidateId)
{
    using namespace sqlite_orm;
    return storage_.get_all<Vote>(
        where(c(&Vote::candidateId) == candidateId),
        order_by(&Vote::id));
}

int VoteDao::countByCandidate(int candidateId)
{
    using namespace sqlite_orm;
    return storage_.count<Vote>(where(c(&Vote::candidateId) == candidateId));
}

bool VoteDao::remove(int id)
{
    if (!storage_.get_optional<Vote>(id))
    {
        return false;
    }
    storage_.remove<Vote>(id);
    return true;
}
