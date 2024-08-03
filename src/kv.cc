
#include "kv.h"

KV* KV::self = nullptr;

KV::KV(int shard_count) 
: _shard_count(shard_count)
, _tso(1)
{
    _shards.reserve(_shard_count);
    for (int i=0; i<_shard_count; ++i) {
        ShardKv* kv = new ShardKv;
        _shards.push_back(kv);
    }
}

KV::~KV() {
    for (int i=0; i<_shard_count; ++i) {
        delete _shards[i];
        _shards[i] = nullptr;
    }
}

