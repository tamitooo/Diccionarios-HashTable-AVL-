#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <string>

using namespace std;

const int maxColision = 3;

// itera sobre el hashtable manteniendo el orden de insercion
template <typename TK, typename TV>
class HashIterator {
private:
    typename vector<pair<TK, TV>>::iterator it;
    typename vector<pair<TK, TV>>::iterator endIt;

public:
    HashIterator(typename vector<pair<TK, TV>>::iterator _it, typename vector<pair<TK, TV>>::iterator _endIt)
        : it(_it), endIt(_endIt) {}

    HashIterator<TK, TV>& operator=(HashIterator<TK, TV> other);
    bool operator!=(HashIterator<TK, TV> other);
    HashIterator<TK, TV>& operator++();   //++it
    pair<TK, TV> operator*();
};

template <typename TK, typename TV>
HashIterator<TK, TV>& HashIterator<TK, TV>::operator=(HashIterator<TK, TV> other) {
    it = other.it;
    endIt = other.endIt;
    return *this;
}

template <typename TK, typename TV>
bool HashIterator<TK, TV>::operator!=(HashIterator<TK, TV> other) {
    return it != other.it;
}

template <typename TK, typename TV>
HashIterator<TK, TV>& HashIterator<TK, TV>::operator++() {
    ++it;
    return *this;
}

template <typename TK, typename TV>
pair<TK, TV> HashIterator<TK, TV>::operator*() {
    return *it;
}

template <typename TK, typename TV>
class HashTable {
public:
    typedef HashIterator<TK, TV> iterator;
    iterator begin() {
        return iterator(insertionOrder.begin(), insertionOrder.end());
    }
    iterator end() {
        return iterator(insertionOrder.end(), insertionOrder.end());
    }

private:
    int capacity;
    int size;
    vector<list<pair<TK, TV>>> buckets;
    vector<TK> keys;
    vector<pair<TK, TV>> insertionOrder;

    int hashFunction(TK key) {
        hash<TK> hasher;
        return hasher(key) % capacity;
    }

public:
    HashTable(int _cap = 5) : capacity(_cap), size(0) {
        buckets.resize(capacity);
    }

    void insert(TK key, TV value) {
        insert(make_pair(key, value));
    }

    void insert(pair<TK, TV> item) {
        int index = hashFunction(item.first);
        for (auto& kv : buckets[index]) {
            if (kv.first == item.first) {
                kv.second = item.second;
                for (auto& p : insertionOrder) {
                    if (p.first == item.first) {
                        p.second = item.second;
                        return;
                    }
                }
            }
        }
        if (buckets[index].size() >= maxColision) {
            rehashing();
            insert(item);
            return;
        }
        buckets[index].push_back(item);
        keys.push_back(item.first);
        insertionOrder.push_back(item);
        size++;
    }

    TV& at(TK key) {
        int index = hashFunction(key);
        for (auto& kv : buckets[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        throw out_of_range("Key not found");
    }

    TV& operator[](TK key) {
        int index = hashFunction(key);
        for (auto& kv : buckets[index]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        TV defaultVal{};
        insert(key, defaultVal);
        return at(key);
    }

    bool find(TK key) {
        int index = hashFunction(key);
        for (auto& kv : buckets[index]) {
            if (kv.first == key) {
                return true;
            }
        }
        return false;
    }

    bool remove(TK key) {
        int index = hashFunction(key);
        auto& bucket = buckets[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                size--;
                for (auto kit = keys.begin(); kit != keys.end(); ++kit) {
                    if (*kit == key) {
                        keys.erase(kit);
                        break;
                    }
                }
                for (auto iit = insertionOrder.begin(); iit != insertionOrder.end(); ++iit) {
                    if (iit->first == key) {
                        insertionOrder.erase(iit);
                        break;
                    }
                }
                return true;
            }
        }
        return false;
    }

    int getSize() {
        return size;
    }

    vector<TK> getAllKeys() {
        return keys;
    }

    vector<pair<TK, TV>> getAllElements() {
        return insertionOrder;
    }

private:
    void rehashing() {
        capacity *= 2;
        vector<list<pair<TK, TV>>> newBuckets(capacity);
        for (auto& bucket : buckets) {
            for (auto& kv : bucket) {
                int newIndex = hash<TK>{}(kv.first) % capacity;
                newBuckets[newIndex].push_back(kv);
            }
        }
        buckets = newBuckets;
    }
};
