#include <iostream>
#include "shard_kv.h"

bool ShardKv::get(const std::string& key, uint64_t start_ts, Value* value) {
    std::unique_lock<std::mutex> lock_guard(_mutex);
    std::cout << "get(" << key
              << ", " << start_ts
              << ")" << std::endl;
    return unsafe_get(key, start_ts, value);
}

bool ShardKv::prewrite(const std::string& key, uint64_t start_ts, 
              LockInfo* lock_info, Value* value) {

    std::unique_lock<std::mutex> lock_guard(_mutex);
    
    bool need_abort = unsafe_check_ww_conflict(key, start_ts);
    if (need_abort) {
        std::cout << "prewrite conflict with key: " << key
                  << " start_ts: " << start_ts << std::endl;
        return false;
    }

    // if really get lock, then need abort
    need_abort = unsafe_get_lock(key, start_ts, nullptr);
    if (need_abort) {
        std::cout << "prewrite hit lock with key: " << key
                  << " start_ts: " << start_ts << std::endl;
        return false;
    }

    unsafe_put_lock(key, lock_info);
    unsafe_put_data(key, start_ts, value);

    return true;
}

bool ShardKv::commit(const std::string& key, LockInfo* lock_info,
            uint64_t commit_ts) {

    std::unique_lock<std::mutex> lock_guard(_mutex);

    bool succ = unsafe_remove_lock(key, lock_info->start_ts, lock_info->lock_type);
    if (lock_info->lock_type == LOCK_TYPE_PRIMARY && !succ) {
        // primary lock missing, commit failed
        return false;
    }

    unsafe_put_write(key, lock_info->start_ts, commit_ts);
    return true;
}

bool ShardKv::unsafe_get(const std::string& key, uint64_t start_ts, Value* value) {
    auto it = _lock.find(key);
    if (it != _lock.end()) {
        auto& lock_info = it->second;
        // small and equal or only small?
        if (lock_info.start_ts <= start_ts) {
            return false;
        }
    }
    auto& version_map = _write[key];
    // no value found
    if (version_map.empty()) {
        return true;
    }

    uint64_t w_commit_ts = 0;
    uint64_t w_start_ts = 0;
    for (auto version_it = version_map.rbegin(); 
                    version_it != version_map.rend(); ++version_it) {
        if (version_it->first > start_ts) {
            // keep search
            continue; 
        }
        // we found
        w_commit_ts = version_it->first;
        w_start_ts = version_it->second;
        break;
    }

    if (w_commit_ts == 0) {
        // true ? not found
        return true;
    }
    unsafe_get_data(key, w_start_ts, value);
    return true;
}

void ShardKv::unsafe_put_data(const std::string& key, uint64_t start_ts, Value* value) {
    std::cout << "unsafe_put_data(" << key 
              << ", " << start_ts 
              << ", " << value->value << ")" << std::endl;
    auto& version_map = _default[key];
    version_map[start_ts] = *value;
}

void ShardKv::unsafe_get_data(const std::string& key, uint64_t start_ts, Value* value) {
    std::cout << "unsafe_get_data(" << key 
              << ", " << start_ts << ")" << std::endl;
    auto& version_map = _default[key];
    *value = version_map[start_ts];
}

bool ShardKv::unsafe_check_ww_conflict(const std::string& key, uint64_t start_ts) {
    auto it = _write.find(key);
    if (it == _write.end()) {
        return false;
    }
    auto& version_map = it->second;
    auto commit_it = version_map.lower_bound(start_ts);
    // there has some commit bigger than this txn's start_ts
    // we should abort
    if (commit_it != version_map.end()) {
        std::cout << "conflict happenned at key: " << key
                  << " start_ts: " << start_ts
                  << " exist bigger write detected: " << commit_it->first << std::endl;
        return true;
    }
    return false;
}

void ShardKv::unsafe_put_lock(const std::string& key, LockInfo* info) {
    _lock[key] = *info;
}

bool ShardKv::unsafe_get_lock(const std::string& key, uint64_t start_ts, LockInfo* info) {
    auto it = _lock.find(key);
    if (it == _lock.end()) {
        return false;
    }
    if (info != nullptr) {
        *info = it->second;
    }
    return true;
}

bool ShardKv::unsafe_remove_lock(const std::string& key, uint64_t start_ts, LockType type) {
    auto it = _lock.find(key);
    if (it == _lock.end()) {
        return false;
    }

    auto& info = it->second;
    if (info.start_ts != start_ts || info.lock_type != type) {
        return false;
    }
    
    _lock.erase(it);
    return true;
}

void ShardKv::unsafe_put_write(const std::string& key, uint64_t start_ts, uint64_t commit_ts) {
    auto& version_map = _write[key];
    // ide check??
    version_map[commit_ts] = start_ts;
}

