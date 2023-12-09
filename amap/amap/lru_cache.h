#pragma once

#include <unordered_map>
#include <list>
// ˫������
struct DoubleLinkedNode {
    int key, value;
    DoubleLinkedNode* prev;
    DoubleLinkedNode* next;
    DoubleLinkedNode() : key(0), value(0), prev(nullptr), next(nullptr) {}
    DoubleLinkedNode(int k, int v) :key(k), value(v), prev(nullptr), next(nullptr) {}
};

typedef enum {
    eIRUNew,
    eIRUReuse,
    eIRUReady,
}eIRUState;

// ÿ��cache�������˫������Ľڵ�, temp[key] = DoubleLinkedNode(key, value)
// �����Ϳ�����get��ʱ��, ���ڵ�key, ��˫��������, �ŵ���ǰ��
// ��putʱ, ���ڵ�key, ֱ�ӷŵ�˫������ǰ��
class LRUCache {
private:
    std::unordered_map<int, DoubleLinkedNode*> temp_;
    int capacity_;
    DoubleLinkedNode* head_ = nullptr;
    DoubleLinkedNode* tail_ = nullptr;
    int size_;
public:
    LRUCache(int capacity) : capacity_(capacity), size_(0) {
        // ������αͷ��αβ���ڵ�
        // head_(0,0)����tail_(0,0)
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

    int get(int key, eIRUState& state) {
        if (temp_.count(key) == 0) // no data in cache
        {
            int ret = -1;
            if (size_ == capacity_) // full
            {
                // remove tile
                DoubleLinkedNode* removedNode = removeTail();
                // return reused index
                ret = removedNode->value;
                //erase the old key
                temp_.erase(removedNode->key);
                // set new key
                temp_[key] = removedNode;
                // reuse, move to head
                addToHead(removedNode);
            }
            else // not full
            {
                // build a new node
                DoubleLinkedNode* node = new DoubleLinkedNode(key, size_);
                // return index
                ret = size_;
                // add node to hashtable
                temp_[key] = node;
                // move the node to head
                addToHead(node);
                size_++;
            }
            return ret;
        }
        else     // has data in cache
        {
            // update key
            DoubleLinkedNode* node = temp_[key];
            moveTohead(node);
            return node->value;
        }
    }

    void put(int key, int value) {
        if (temp_.count(key) == 0) {
            DoubleLinkedNode* node = new DoubleLinkedNode(key, value);
            temp_[key] = node;
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
            DoubleLinkedNode* node = temp_[key];
            node->value = value;
            moveTohead(node);
        }
    }

    // �ڵ�ӵ���ǰ��
    void addToHead(DoubleLinkedNode* node) {
        // head_ ���� node ���� head_->next
        node->prev = head_;
        node->next = head_->next;
        head_->next->prev = node;
        head_->next = node;
    }

    // �Ƴ��ڵ�
    void removeNode(DoubleLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // ɾ��β���ڵ�
    DoubleLinkedNode* removeTail() {
        DoubleLinkedNode* node = tail_->prev;
        removeNode(node);
        return node;
    }

    // �ƶ��ڵ㵽ͷ
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
