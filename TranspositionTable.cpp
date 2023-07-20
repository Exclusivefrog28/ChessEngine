#include <cstdio>
#include "TranspositionTable.h"

#define MATE_SCORE 65536

TranspositionTable::Entry TranspositionTable::getEntry(long int key, int ply) {
    int index = abs(key) % TT_SIZE;
    Entry entry = entries[index];

    if (abs(entry.score) == MATE_SCORE) {
        int sign = entry.score > 0 ? 1 : -1;
        entry.score = sign * (MATE_SCORE - ply);
    }
    reads++;

    return entry;
}

bool TranspositionTable::contains(long int key) {
    int index = abs(key) % TT_SIZE;
    bool exists = entries[index].nodeType != EMPTY;
    bool sameKey = (entries[index].key == key);
    if(exists && !sameKey) collisions++;

    return exists && sameKey;
}

void TranspositionTable::setEntry(long int key, TranspositionTable::Entry entry, int ply) {
    int index = abs(key) % TT_SIZE;

    if (abs(entry.score) >= MATE_SCORE - (entry.depth + ply)) {
        int sign = entry.score > 0 ? 1 : -1;
        entry.score = sign * MATE_SCORE;
    }

    NodeType savedType = entries[index].nodeType;
    NodeType newType = entry.nodeType;

    //REPLACEMENT SCHEME
    // 1. Prefer EXACT nodes to bounds
    // 2. Prefer deeper nodes to shallower
    if (savedType != EMPTY) {
        if ((savedType != EXACT && newType != EXACT) || (savedType == EXACT && newType == EXACT)) {
            if (entries[index].depth <= entry.depth) write(index, entry);
        } else if (savedType != EXACT) write(index, entry);
    } else write(index, entry);
}

void TranspositionTable::write(int index, TranspositionTable::Entry entry) {
    entries[index] = entry;
    writes++;
}

void TranspositionTable::resetCounters() {
    reads = 0;
    writes = 0;
    collisions = 0;
}

