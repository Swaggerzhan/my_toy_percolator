
#include <functional>
#include <atomic>
#include <vector>
#include "shard_kv.h"

class KV {
public:

    static KV* getInstance() {
        if (self == nullptr) {
            self = new KV();
        }
        return self;
    }

    ShardKv* pick_shard(const std::string& key) {
        std::hash<std::string> hasher;
        int idx = static_cast<int>(hasher(key))
            % _shards.size();
        return _shards[idx];
    }

    uint64_t get_ts() {
        return _tso.fetch_add(1);
    }

private:

    KV(int shard_count=2);
    ~KV();

private:
    // never mind the thread safe, just a mock program
    static KV* self;

    // just for mock, not consider the rebalance
    std::vector<ShardKv*> _shards;
    int _shard_count;

    std::atomic<uint64_t> _tso;
};

#define g_kv KV::getInstance()

