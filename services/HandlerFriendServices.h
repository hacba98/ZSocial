/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HandlerFriendServices.h
 * Author: cpu10855
 *
 * Created on July 2, 2019, 2:54 PM
 */

#ifndef HANDLERFRIENDSERVICES_H
#define HANDLERFRIENDSERVICES_H

#include "Poco/Util/Application.h"
#include "Poco/Logger.h"
#include "Poco/Timestamp.h"

#include <vector>
#include <thrift/TToString.h>

#include "../src/gen-cpp/FriendServices.h"
#include "../db/SubKC.h"
#include "../util/Converter.h"

class HandlerFriendServices : public FriendServicesIf {
public:
	HandlerFriendServices();
	HandlerFriendServices(const HandlerFriendServices& orig);
	~HandlerFriendServices();
	
	// Friend function API
	void checkRequest(pingResult& _return, const int32_t id);
	ErrorCode::type addFriend(const FriendRequest& request);
	ErrorCode::type acceptRequest(const int32_t curId, const int32_t requestId);
	ErrorCode::type declineRequest(const int32_t curId, const int32_t requestId);
	ErrorCode::type removeFriend(const int32_t curId, const int32_t friendId);
	void viewFriendList(listFriendResult& _return, const int32_t id, const int32_t index, const int32_t size);

private:
	// variables add later
	SubKC& _kc = Poco::Util::Application::instance().getSubsystem<SubKC>();
	
};

#endif /* HANDLERFRIENDSERVICES_H */

