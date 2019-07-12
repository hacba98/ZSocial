/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   userProfileHandler.h
 * Author: cpu02331
 *
 * Created on May 21, 2019, 3:26 PM
 */

#ifndef USERPROFILEHANDLER_H
#define USERPROFILEHANDLER_H

#include "src/gen-cpp/ProfileServices.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "cache/LruCache.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include "Poco/Notification.h"
#include "Poco/NotificationQueue.h"

#include "util/Worker.h"
#include "db/userProfileDB.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include <iostream>
#include <mutex>
#include <vector>
#include "Poco/Util/Subsystem.h"

#include "Poco/HMACEngine.h"
#include "Poco/SHA1Engine.h"

using Poco::DigestEngine;
using Poco::HMACEngine;
using Poco::SHA1Engine;

using Poco::Notification;
using Poco::NotificationQueue;
using Poco::ThreadPool;
using Poco::Runnable;
using Poco::AutoPtr;

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

class SocialServiceHandler : virtual public ProfileServicesIf,public Poco::Util::Subsystem {
public:
    
    SocialServiceHandler() ;

    ~SocialServiceHandler();

    void Login(loginResult& _return, const std::string& username, const std::string& password);
    ErrorCode::type Logout(const int32_t userId);
    void CreateProfile(CreateUserResult& _return, const UserProfile& profile);
    void GetProfile(GetUserResult& _return, const int32_t userId);
    ErrorCode::type UpdateProfile(const UserProfile& profile, const int32_t userId);
    ErrorCode::type DeleteProfile(const int32_t userId);
    void ansyCreateProfile(CreateUserResult& _return, const UserProfile& profile);
    ErrorCode::type ansyUpdateProfile(const UserProfile& profile, const int32_t userId);
    ErrorCode::type ansyDeleteProfile(const int32_t userId);
    void getList(ListProfileResult& _return, const std::vector<int32_t> & ids);
    int32_t GetIdByName(const std::string& username);
    bool chechExist(const int32_t userId);
    
    void CreateWithId(int userId,const UserProfile& _profile);
    int GetSimpleProfile(SimpleProfile& _ret,int32_t userId);
    
    std::string hash(const std::string& key , int id){
        std::string pass = key;
        pass.append(to_string(id));
        
        HMACEngine<SHA1Engine> hmac("LOL");
        hmac.update(pass);
        
        std::string hashPass(DigestEngine::digestToHex(hmac.digest()));
        return hashPass;
    }
private:
    int newID;
    std::mutex mutexUpdate;
    Poco::Logger* log;
    Poco::Timestamp now;
    
    UserProfileDB* db;
    
    lruCache<int, UserProfile> users;
    //int hit,all;
    
    NotificationQueue* profileQueue;
    
    
protected:
    
    void initialize ( Poco::Util::Application &self );
    void reinitialize ( Poco::Util::Application &self );
    void uninitialize();

    const char* name() const;

};

#endif /* USERPROFILEHANDLER_H */

