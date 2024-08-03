#include <iostream>
#include "txn.h"

void Txn::init(KeyValuePairVec& inputs) {
    _vec.swap(inputs);
}

void Txn::init_lock() {
    for (int i=0; i<_vec.size(); i++) {
        bool primary = (i == 0);
        _locks.emplace_back(
                primary ? LOCK_TYPE_PRIMARY : LOCK_TYPE_SECONDARY,
                _vec[0].key, _start_ts);
    }
}

bool Txn::prewrite() {
    _start_ts = g_kv->get_ts();
    init_lock();
    for (int i=0; i<_vec.size(); i++) {
        auto& tuple = _vec[i];
        auto& lock = _locks[i];
        ShardKv* shard_kv = g_kv->pick_shard(tuple.key);

        bool succ = shard_kv->prewrite(tuple.key, _start_ts, &lock, &tuple.value);
        // TODO: rollback??
        if (!succ) {
            return false;
        }
    }        
    return true;
    
}

bool Txn::commit() {
    _commit_ts = g_kv->get_ts();
    for (int i=0; i<_vec.size(); i++) {
        bool primay = (i == 0);
        auto& input = _vec[i];
        ShardKv* shard_kv = g_kv->pick_shard(input.key);

        auto& info = _locks[i];
        bool succ = shard_kv->commit(input.key, &info, _commit_ts);
        if (primay && !succ) {
            // failed
        }
        // TODO: ??
        if (!succ) {
            return false;
        }
    }
    return true;

}

bool Peek::read(const std::string& key, std::string& value) {
    _start_ts = g_kv->get_ts();
    ShardKv* shard_kv = g_kv->pick_shard(key);
    Value tmp_value;
    bool succ = shard_kv->get(key, _start_ts, &tmp_value);
    if (succ && !tmp_value.value.empty()) {
        value = tmp_value.value;
    }
    return succ;
}

