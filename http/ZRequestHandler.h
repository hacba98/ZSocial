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
#include "Poco/ObjectPool.h"
#include "Poco/Util/Application.h"

#include "../gen-cpp/ProfileServices.h"

#include "Connection.h"

#include <iostream>
#include <string>

class ZRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
	ZRequestHandlerFactory(){
		int poolCapacity = Poco::Util::Application::instance().config().getInt("http.connectionPoolCapacity", 50);
		int poolPeakCapacity = Poco::Util::Application::instance().config().getInt("http.connectionPoolPeakCapacity", 100);
		
		boost::shared_ptr<Poco::ObjectPool<ProfileConnection> > pool_profiles(new Poco::ObjectPool<ProfileConnection>(poolCapacity, poolPeakCapacity));
		boost::shared_ptr<Poco::ObjectPool<FriendConnection> > pool_friends(new Poco::ObjectPool<FriendConnection>(poolCapacity, poolPeakCapacity));
		
		ZRequestHandlerFactory::_pool_profiles = pool_profiles;
		ZRequestHandlerFactory::_pool_friends = pool_friends;	
	}
	
	virtual Poco::Net::HTTPRequestHandler * createRequestHandler(const Poco::Net::HTTPServerRequest &req);
	
	static Poco::ObjectPool<ProfileConnection> * profilePool(){return ZRequestHandlerFactory::_pool_profiles.get();}
	static Poco::ObjectPool<FriendConnection> * friendPool(){ return ZRequestHandlerFactory::_pool_friends.get();}
	
private:
	static boost::shared_ptr<Poco::ObjectPool<ProfileConnection> > _pool_profiles;
	static boost::shared_ptr<Poco::ObjectPool<FriendConnection> > _pool_friends;
};

class NoServicesInvokeHandler : public Poco::Net::HTTPRequestHandler {
public:
	void handleRequest(
		Poco::Net::HTTPServerRequest &req, 
		Poco::Net::HTTPServerResponse &res);
};

class ProfileRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
	ProfileRequestHandler(ProfileConnection *client): _conn(client){}
	~ProfileRequestHandler(){
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
private:
	ProfileConnection *_conn;
};

class FriendRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
	FriendRequestHandler(FriendConnection *borrow): _conn(borrow){}
	~FriendRequestHandler(){
		ZRequestHandlerFactory::friendPool()->returnObject(_conn);
	}
	
	// main control
	void handleRequest(
		Poco::Net::HTTPServerRequest &req, 
		Poco::Net::HTTPServerResponse &res){};
	
	// handler of specify request
	
private:
	FriendConnection *_conn;
};



#endif /* ZREQUESTHANDLER_H */

