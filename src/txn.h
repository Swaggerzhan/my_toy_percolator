#include "kv.h"

// only for write
class Txn {
public:

    void init(KeyValuePairVec& inputs);

    void init_lock();

    bool prewrite();

    bool commit();

private:
    uint64_t _start_ts;
    uint64_t _commit_ts;
    KeyValuePairVec _vec;
    std::vector<LockInfo> _locks;
};

// read
class Peek {
public:
    bool read(const std::string& key, std::string& value);

private:
    uint64_t _start_ts;
};
