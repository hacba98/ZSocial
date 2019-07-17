/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRequestHandler.h
 * Author: cpu10855
 *
 * Created on July 4, 2019, 10:04 AM
 */

#ifndef ZREQUESTHANDLER_H
#define ZREQUESTHANDLER_H

#include <boost/smart_ptr/shared_ptr.hpp>

#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Net/HTTPCookie.h"
#include "Poco/ObjectPool.h"
#include "Poco/Util/Application.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeParser.h"
#include "Poco/FileStream.h"
#include "Poco/Format.h"
#include "Poco/NumberFormatter.h"

#include "thrift/TToString.h"

#include "../gen-cpp/ProfileServices.h"
#include "../gen-cpp/FriendServices.h"
#include "../gen-cpp/NewsFeedService.h"

#include "Connection.h"

#include <iostream>
#include <string>

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;
class ZRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:

    ZRequestHandlerFactory() {
        int poolCapacity = Poco::Util::Application::instance().config().getInt("http.connectionPoolCapacity", 50);
        int poolPeakCapacity = Poco::Util::Application::instance().config().getInt("http.connectionPoolPeakCapacity", 100);

        boost::shared_ptr<Poco::ObjectPool<ProfileConnection> > pool_profiles(new Poco::ObjectPool<ProfileConnection>(poolCapacity, poolPeakCapacity));
        boost::shared_ptr<Poco::ObjectPool<FriendConnection> > pool_friends(new Poco::ObjectPool<FriendConnection>(poolCapacity, poolPeakCapacity));
        boost::shared_ptr<Poco::ObjectPool<NewsFeedConnection> > pool_newsfeed(new Poco::ObjectPool<NewsFeedConnection>(poolCapacity, poolPeakCapacity));

        ZRequestHandlerFactory::_pool_profiles = pool_profiles;
        ZRequestHandlerFactory::_pool_friends = pool_friends;
        ZRequestHandlerFactory::_pool_newsfeed = pool_newsfeed;
    }

    virtual Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest &req);

    static Poco::ObjectPool<ProfileConnection> * profilePool() {
        return ZRequestHandlerFactory::_pool_profiles.get();
    }

    static Poco::ObjectPool<FriendConnection> * friendPool() {
        return ZRequestHandlerFactory::_pool_friends.get();
    }

    static Poco::ObjectPool<NewsFeedConnection> * newsfeedPool() {
        return ZRequestHandlerFactory::_pool_newsfeed.get();
    }

private:
    static boost::shared_ptr<Poco::ObjectPool<ProfileConnection> > _pool_profiles;
    static boost::shared_ptr<Poco::ObjectPool<FriendConnection> > _pool_friends;
    static boost::shared_ptr<Poco::ObjectPool<NewsFeedConnection> > _pool_newsfeed;
};

class NoServicesInvokeHandler : public Poco::Net::HTTPRequestHandler {
public:

    void handleRequest(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);
};

class ProfileRequestHandler : public Poco::Net::HTTPRequestHandler {
public:

    ProfileRequestHandler(ProfileConnection *client) : _conn(client) {
    }

    ~ProfileRequestHandler() {
        ZRequestHandlerFactory::profilePool()->returnObject(_conn);
    }

    // main control
    void handleRequest(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);

    // handler of specify request
    void handleLogin(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);

    void handleRegister(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);
    
    void handleUpdate(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);
    
    void handleLogout(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);
private:
    ProfileConnection *_conn;
};

class FriendRequestHandler : public Poco::Net::HTTPRequestHandler {
public:

    FriendRequestHandler(FriendConnection *borrow) : _conn(borrow) {
    }

    ~FriendRequestHandler() {
        ZRequestHandlerFactory::friendPool()->returnObject(_conn);
    }

    // main control

    void handleRequest(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res) {
    };

    // handler of specify request

private:
    FriendConnection *_conn;
};

class NewsFeedRequestHandler : public Poco::Net::HTTPRequestHandler {
public:

    NewsFeedRequestHandler(NewsFeedConnection *borrow) : _conn(borrow) {
    }

    ~NewsFeedRequestHandler() {
        ZRequestHandlerFactory::newsfeedPool()->returnObject(_conn);
    }

    void handleRequest(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);
    void handleCreateRequest(Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res);
private:
    NewsFeedConnection *_conn;
};

class TOOL {
public:

    static int getTime(int d, int m, int y) {
        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_year = y - 1900;
        timeinfo->tm_mon = m - 1;
        timeinfo->tm_mday = d;

        return int(mktime(timeinfo));
    }

    static void getDMY(int birth, int &d, int &m, int &y) {
        time_t b = (time_t) birth;
        tm *ltm = localtime(&b);
        d = ltm->tm_mday;
        m = ltm->tm_mon + 1;
        y = ltm->tm_year + 1900;
    }

    static void setProfile(UserProfile& profile, std::string name, std::string gender, int birth, long phoneNumber, std::string username, std::string password) {
        if (birth != 0) profile.__set_birth(birth);
        if (gender != "") profile.__set_gender(gender == "Nam");
        if (name != "No one will name like this") profile.__set_name(name);
        if (password != "") profile.__set_password(password);
        if (phoneNumber != 0l)profile.__set_phoneNumber(phoneNumber);
        if (username != "") profile.__set_username(username);
    };
};

#endif /* ZREQUESTHANDLER_H */

