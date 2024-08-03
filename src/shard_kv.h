
#include <mutex>
#include <map>
#include "tuple.h"


class ShardKv {
public:

    bool get(const std::string& key, uint64_t start_ts, Value* value);

    bool prewrite(const std::string& key, uint64_t start_ts, 
                  LockInfo* lock_info, Value* value);

    bool commit(const std::string& key, LockInfo* lock_info,
                uint64_t commit_ts);

private:

    bool unsafe_get(const std::string& key, uint64_t start_ts, Value* value);

    // for _default
    void unsafe_put_data(const std::string& key, uint64_t start_ts, Value* value);
    void unsafe_get_data(const std::string& key, uint64_t start_ts, Value* value);
    bool unsafe_check_ww_conflict(const std::string& key, uint64_t start_ts);

    // for _lock
    void unsafe_put_lock(const std::string& key, LockInfo* info);
    bool unsafe_get_lock(const std::string& key, uint64_t start_ts, LockInfo* info);
    bool unsafe_remove_lock(const std::string& key, uint64_t start_ts, LockType type);

    // for _write
    void unsafe_put_write(const std::string& key, uint64_t start_ts, uint64_t commit_ts);

private:

    // for mock atomic change of CF
    std::mutex _mutex;
    // CF: default
    std::map<std::string, std::map<uint64_t, Value>> _default;
    // CF: lock
    std::map<std::string, LockInfo> _lock;
    // CF: write
    std::map<std::string, std::map<uint64_t, uint64_t>> _write;


};


