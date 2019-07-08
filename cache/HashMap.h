/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   newfile.h
 * Author: cpu02331
 *
 * Created on July 1, 2019, 9:06 AM
 */

#ifndef ___HASH_MAP_H
#define ___HASH_MAP_H
#include <stdio.h>
#include <Poco/SharedPtr.h>

template<class Key,class Value>
class entry{
public:
    Key key;
    Value val;
    
    entry(){}
    entry(Key _key,const Value _val):key(_key),val(_val){}
};

template<class Key , class Value>
class HashMap{
    public:
        HashMap(int _m = 102031):maxLength(_m){
            vals = new Poco::SharedPtr<entry<Key,Value>>[maxLength];        
        }
        ~HashMap(){
            delete[] vals;
        }
        
        bool insert(Key k ,const Value v){
            int hashVal = k;
            
            if (checkKey(k)) return false;
            
            do{
                hashVal = hashFunc(hashVal);
            }while(!vals[hashVal].isNull());
            
            vals[hashVal] = Poco::SharedPtr<entry<Key,Value>>(new entry<Key,Value>(k,v));
            
            return false;
        }
        
        bool remove(Key k){
            int hashVal = hashFunc(k);
            while(!vals[hashVal].isNull()){
                if(vals[hashVal]->key == k){
                    vals[hashVal] = NULL;
                    return true;
                }else{
                    hashVal = hashFunc(hashVal);
                }
            }
            return false;
        }
        
        bool find(Key k, Value& val){
            int hashVal = hashFunc(k);
            while(!vals[hashVal].isNull()){
                if(vals[hashVal]->key == k){
                    val = vals[hashVal]->val;
                    return true;
                }else{
                    hashVal = hashFunc(hashVal);
                }
            }
            return false;
        }
        //USE AT YOUR OWN RISK ,IF KET NO EXIST THEN throw ex
        Value& at(Key k){
            int hashVal = hashFunc(k);
            while(!vals[hashVal].isNull()){
                if(vals[hashVal]->key == k){
                    return vals[hashVal]->val;
                }else{
                    hashVal = hashFunc(hashVal);
                }
            }
            throw std::out_of_range("no exist key");
        }
        
        bool checkKey(Key k){
            int hashVal = hashFunc(k);
            
            while(!vals[hashVal].isNull()){
                
                if(vals[hashVal]->key == k){
                    return true;
                }else{
                    hashVal = hashFunc(hashVal);
                }
            }
            return false;
            
        }
private:
    int maxLength;
    
    Poco::SharedPtr<entry<Key,Value>>* vals;
    
    int hashFunc(Key k){
        
        return k * 7 % maxLength;
    }
};

#endif /* NEWFILE_H */

