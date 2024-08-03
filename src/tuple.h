#include <string>
#include <vector>
#include <map>

enum LockType {
    LOCK_TYPE_PRIMARY = 0,
    LOCK_TYPE_SECONDARY = 1,
};

struct MvccKey {
    std::string key;
    uint64_t start_ts;
};

struct LockInfo {
    LockType lock_type;
    std::string primary_lock_key;
    uint64_t start_ts;

    LockInfo() {}

    LockInfo(LockType lock_type_,
             const std::string& key_,
             uint64_t start_ts_) 
    : lock_type(lock_type_)
    , primary_lock_key(key_)
    , start_ts(start_ts_) 
    {}
};

// same as MvccKey
struct WriteInfo {
    std::string key;
    uint64_t commit_ts;
};

struct Value {
    std::string value;
};

struct KeyValuePair {
    std::string key; 
    Value value;
};

typedef std::vector<KeyValuePair> KeyValuePairVec;
