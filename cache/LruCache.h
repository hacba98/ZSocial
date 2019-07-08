/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LruCache.h
 * Author: cpu02331
 *
 * Created on May 20, 2019, 3:55 PM
 */

#ifndef LRUCACHE_H
#define LRUCACHE_H


#include "cache/HashMap.h"

#include <Poco/Mutex.h>

using namespace std;

template <class T>
class node{
public:
    T pre,nxt;
    node(){
    }
};

template <class Key, class Value>
class lruCache {
public:

    lruCache(int n = 100) : maxLength(n),length(0) {
        
    }

    bool add(Key key,const Value& val) {
        scopedLock l(m);
        
        if (table.checkKey(key)){
            if (length <= 1 ) return false;
            goTop(key);
            //printCahe();
            return false;
        }
        
        table.insert(key, val);
        pushLru(key);
        
        ++length;
        
        if (length >= maxLength) {
            for(int count = 0; count <= maxLength / 10 ; ++count){
                table.remove(tail);
                removeLru(tail);
                --length;
            }
        }
        //printCahe();
        return true;
    }

    bool update(Key key,const Value& val) {
        remove(key);
        
        return add(key, val);
        //printCahe();
    };

    bool remove(Key key) {
        scopedLock l(m);
        if (length == 0) return true;
        
        if (table.checkKey(key)){
            length--;
            table.remove(key);
            removeLru(key);
            //printCahe();
            return true;
        }else
            return false;
    };

    int get(Key key , Value& _ret) {
        scopedLock l(m);
        
        if (table.find(key,_ret)){
            if (length <= 1) return 1;
            goTop(key);
            //printCahe();
            return 1;
        }else
            return 0;
    };
    void setBound(int m){
        maxLength = m;
    }
private:
    HashMap<Key, Value> table;
    HashMap<Key, node<Key>> lru;
    
    mutable Poco::FastMutex m;
    typedef typename Poco::FastMutex::ScopedLock scopedLock;
    
    int maxLength ,length;
    Key head,tail;
    
    void printCahe(){
        Key i = head;
        while (i != tail){
            printf("%d ", i);
            i = lru.at(i).pre;
        }
        printf("%d\n",i);
    }
    
    void pushLru(Key k){
        if(length == 0){
            node<Key> n;
            lru.insert(k,n);
            head = tail = k;
        }else{
            node<Key> n;
            n.pre = head;
            lru.at(head).nxt = k;
            lru.insert(k,n);
            head = k;
        }
    }
    void goTop(Key k){
        
            if (k == head){
                return;
            }else if(k == tail){
                tail = lru.at(tail).nxt;
                lru.at(k).pre = head;
                lru.at(head).nxt = k;
                head = k;
            }else{
                node<Key>& n = lru.at(k);
                lru.at(n.nxt).pre = n.pre;
                lru.at(n.pre).nxt = n.nxt;
                n.pre = head;
                lru.at(head).nxt = k;
                head = k;
            }
        
    }
    void removeLru(Key k){
        
            if (k == head){
                head = lru.at(head).pre;
                lru.remove(k);
            }else if(k == tail){
                tail = lru.at(tail).nxt;
                lru.remove(k);
            }else{
                node<Key> n = lru.at(k);
                lru.at(n.nxt).pre = n.pre;
                lru.at(n.pre).nxt = n.nxt;
                lru.remove(k);
            }
        
    }
};

#endif