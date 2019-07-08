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
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <iostream>
#include <mutex>
#include <vector>
#include <Poco/Util/Subsystem.h>

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

    int32_t Login(const std::string& username, const std::string& password);
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
    
    void CreateWithId(int userId,const UserProfile& profile);
    int GetSimpleProfile(SimpleProfile& _ret,int32_t userId);
    
    void checkRequest(pingResult& _return, const int32_t id) {};
    ErrorCode::type addFriend(const FriendRequest& request) {};
    ErrorCode::type acceptRequest(const int32_t curId, const int32_t requestId) {};
    ErrorCode::type declineRequest(const int32_t curId, const int32_t requestId) {};
    ErrorCode::type removeFriend(const int32_t curId, const int32_t friendId) {};
    void viewFriendList(listFriendResult& _return, const int32_t id, const int32_t index, const int32_t size) {};

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

