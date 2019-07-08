/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   userProfileDB.h
 * Author: cpu02331
 *
 * Created on May 23, 2019, 2:01 PM
 */

#ifndef USERPROFILEDB_H
#define USERPROFILEDB_H

#include "src/gen-cpp/ProfileServices.h"
#include "Poco/Logger.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include <kchashdb.h>
#include <iostream>
#include <Poco/Util/Subsystem.h>
#include <Poco/Util/Application.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

class UserProfileDB : public Poco::Util::Subsystem{
public:
    UserProfileDB();
    ~UserProfileDB();
    
    int getNextID();
    
    int setNextID(int newID);
    
    bool isContainDB(int id) ;
    
    ErrorCode::type setDB(int id, UserProfile profile) ;

    ErrorCode::type getDB(int id, UserProfile &profile) ;

    ErrorCode::type removeDB(int id) ;
    
    ErrorCode::type setUsername(std::string username , int id);
    ErrorCode::type rmUsername(std::string username );
    ErrorCode::type getIdByName(int& _ret,std::string username);
    
private:
    kyotocabinet::HashDB db;
    kyotocabinet::HashDB keyStore;
    kyotocabinet::HashDB username_id;
    
    Poco::Logger* log;
    
    std::string serializeProfile(UserProfile obj) ;
    void deserilizeProfile(std::string serializeString , UserProfile &profile);
protected:
    
    void initialize ( Poco::Util::Application &self );
    void reinitialize ( Poco::Util::Application &self );
    void uninitialize();

    const char* name() const;
};

#endif /* USERPROFILEDB_H */

