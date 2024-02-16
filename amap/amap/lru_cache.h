#pragma once

#include <unordered_map>
#include <list>
#include <assert.h>
#include "tile_id.h"

// 双向链表
struct DoubleLinkedNode {
    tileId id;
    int key, value;
    DoubleLinkedNode* prev;
    DoubleLinkedNode* next;
    DoubleLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
    DoubleLinkedNode(int k, int v) :key(k), value(v), prev(nullptr), next(nullptr) {}
};

typedef enum {
    eIRUFresh,
    eIRUReuse,
    eIRUReady,
}eIRUState;

typedef struct tagIRUState {
    eIRUState state;
    tileId oldId;
}sIRUState;

// 每个cache保存的是双向链表的节点, temp[key] = DoubleLinkedNode(key, value)
// 这样就可以在get的时候, 将节点key, 从双向链表中, 放到最前面
// 在put时, 将节点key, 直接放到双向链表前面
class LRUCache {
private:
    std::unordered_map<int, DoubleLinkedNode*> temp_;
    int capacity_;
    DoubleLinkedNode* head_ = nullptr;
    DoubleLinkedNode* tail_ = nullptr;
    int size_;
public:
    LRUCache(int capacity) : capacity_(capacity), size_(0) {
        // 创建个伪头和伪尾部节点
        // head_(0,0)←→tail_(0,0)
        this->head_ = new DoubleLinkedNode();
        this->tail_ = new DoubleLinkedNode();
        this->head_->next = this->tail_;
        this->tail_->prev = this->head_;
    }
    ~LRUCache()
    {
        while (size_)
        {
            DoubleLinkedNode* removedNode = removeTail();
            temp_.erase(removedNode->key);
            delete removedNode;
            size_--;
        }
    }
    int getindex(tileId& id) {
        if (temp_.count(id.getKey()) == 0) // no data in cache
        {
            return -1;
        }
        else {
            DoubleLinkedNode* node = temp_[id.getKey()];
            if (id != node->id) {
                int a = 0;
            }
            assert(id == node->id);
            return node->value;
        }
    }
    int get(tileId& id, sIRUState& state) {
        if (temp_.count(id.getKey()) == 0) // no data in cache
        {
            int ret = -1;
            if (size_ == capacity_) // full
            {
                // remove tile
                DoubleLinkedNode* removedNode = removeTail();

                // return reused index
                ret = removedNode->value;
                state.oldId = removedNode->id;

                //erase the old key
                temp_.erase(removedNode->key);

                removedNode->key = id.getKey();    // update key, fix key error
                removedNode->id = id;              // update id, fix key error

                // set new key
                temp_[id.getKey()] = removedNode;
                // reuse, move to head
                addToHead(removedNode);

                state.state = eIRUReuse;
            }
            else // not full
            {
                // build a new node
                DoubleLinkedNode* node = new DoubleLinkedNode(id.getKey(), size_);
                node->id = id;

                // return index
                ret = size_;
                // add node to hashtable
                temp_[id.getKey()] = node;
                // move the node to head
                addToHead(node);
                size_++;
                state.state = eIRUFresh;
            }
            return ret;
        }
        else     // has data in cache
        {
            // update key
            DoubleLinkedNode* node = temp_[id.getKey()];
            if (id != node->id) {
                int a = 0;
            }
            assert(id == node->id);

            moveTohead(node);

            state.state = eIRUReady;
            state.oldId = node->id;

            return node->value;
        }
    }

    void put(tileId id, int value) {
        if (temp_.count(id.getKey()) == 0) {
            DoubleLinkedNode* node = new DoubleLinkedNode(id.getKey(), value);
            node->id = id;
            temp_[id.getKey()] = node;
            addToHead(node);
            size_++;
            if (size_ > capacity_) {
                DoubleLinkedNode* removedNode = removeTail();
                temp_.erase(removedNode->key);
                delete removedNode;
                size_--;
            }
        }
        else {
            DoubleLinkedNode* node = temp_[id.getKey()];
            node->value = value;
            moveTohead(node);
        }
    }

    // 节点加到最前面
    void addToHead(DoubleLinkedNode* node) {
        // head_ ←→ node ←→ head_->next
        node->prev = head_;
        node->next = head_->next;
        head_->next->prev = node;
        head_->next = node;
    }

    // 移除节点
    void removeNode(DoubleLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // 删除尾部节点
    DoubleLinkedNode* removeTail() {
        DoubleLinkedNode* node = tail_->prev;
        removeNode(node);
        return node;
    }

    // 移动节点到头
    void moveTohead(DoubleLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
