/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HandlerFriendServices.cpp
 * Author: cpu10855
 * 
 * Created on July 2, 2019, 2:54 PM
 */

#include "HandlerFriendServices.h"

using namespace Poco;
using namespace Poco::Util;
using namespace std;

HandlerFriendServices::HandlerFriendServices() {
}

HandlerFriendServices::HandlerFriendServices(const HandlerFriendServices& orig) {
}

HandlerFriendServices::~HandlerFriendServices() {
}

// ----- Implement services
void HandlerFriendServices::checkRequest(pingResult& _return, const int32_t id) {
//	Application::instance().logger().information("Checking friend pending request.");
//
//	// check user existed
//	string uid = to_string(id);
//	if (!_kc.checkUserExisted(uid)) {
//		_return.haveData = false;
//		return;
//	}
//
//	// get
//	try {
//		std::set<int32_t> tmp;
//		_kc.loadPending(uid, tmp);
//
//		// case that don't have any data
//		if (tmp.empty()) {
//			_return.haveData = false;
//			return;
//		}
//
//		for (std::set<int32_t>::iterator it = tmp.begin(); it != tmp.end(); it++) {
//			FriendRequest req;
//			int32_t reqid = *it;
//			// load in cache first
//			if (_cache.get<int32_t, FriendRequest>("friend.cache.friendRequest", reqid, req)) {
//				// found in cache
//			} else {
//				// not found
//				string value;
//				_kc.loadRequest(to_string(*it), value);
//				_convert_req.deserialize(value, req);
//			}
//			_return.data.push_back(req);
//			// TODO change set<i32> to list<Request> - DONE
//		}
//
//		_return.haveData = true;
//	} catch (...) {
//		Application::instance().logger().error("Something went wrong");
//	}

	return;
}

ErrorCode::type HandlerFriendServices::addFriend(const FriendRequest& request) {
//	Application::instance().logger().information("Incoming add friend request");
//
//	// check user existed
//	string p_send_id = to_string(request.p_send_req);
//	string p_recv_id = to_string(request.p_recv_req);
//	if (!_kc.checkUserExisted(p_send_id) || !_kc.checkUserExisted(p_recv_id)) {
//		return ErrorCode::USER_NOT_FOUND;
//	}
//
//	// check request already send
//	{
//		set<int32_t> cachePending;
//		int32_t key = request.p_recv_req;
//
//		if (_cache.get<int32_t, set < int32_t >> ("friend.cache.pendingUserId", key, cachePending)) {
//			// in cache
//			if (cachePending.find(request.p_send_req) != cachePending.end())
//				return ErrorCode::DUPLICATED_REQUEST;
//		} else {
//			// not found in cache
//			_kc.loadPendingUser(p_recv_id, cachePending);
//			if (cachePending.find(request.p_send_req) != cachePending.end())
//				return ErrorCode::DUPLICATED_REQUEST;
//		}
//	}
//
//	// check user not friend 
//	{
//		std::set<int32_t> tmp;
//		_kc.loadFriendList(p_recv_id, tmp); // get all friend list
//		if (tmp.find(request.p_send_req) != tmp.end())
//			return ErrorCode::DUPLICATED_REQUEST;
//	}
//
//	// store request in request DB
//	FriendRequest new_request(request);
//	int32_t key_request = _kc.generateRequestId();
//	string skey_request = to_string(key_request);
//	string value;
//	new_request.id = key_request;
//
//	// get current time
//	Poco::Timestamp now;
//	new_request.time = static_cast<int32_t> (now.epochTime());
//
//	// serialize object into string
//	_convert_req.serialize(new_request, value);
//
//	// store in DB
//	_kc.store(skey_request, value, SubKC::DB_TYPE::REQUEST);
//
//	// store request key in pending DB
//	_kc.store(p_recv_id, skey_request, SubKC::DB_TYPE::PENDING);
//	_kc.store(p_recv_id, p_send_id, SubKC::DB_TYPE::PENDING_USER);
//
//	// cache 1-request cache. 2-pending cache
//	std::set<int32_t> tmp;
//	_kc.loadPendingUser(p_recv_id, tmp);
//	tmp.insert(request.p_send_req);
//	_cache.put<int32_t, FriendRequest>("friend.cache.friendRequest", key_request, new_request);
//	_cache.put<int32_t, set < int32_t >> ("friend.cache.pendingUserId", request.p_recv_req, tmp);
//
//	return ErrorCode::type::SUCCESS;
}

ErrorCode::type HandlerFriendServices::acceptRequest(const int32_t curId, const int32_t requestId) {
//	Application::instance().logger().information("Add friend request");
//
//	// check user existed
//	string uid = to_string(curId);
//	string reqid = to_string(requestId);
//	bool userInCache = _cache.check<string, User>("friend.cache.user", uid);
//	bool requestInCache = _cache.check<int32_t, FriendRequest>("friend.cache.friendRequest", requestId);
//
//	// user not found
//	if (!(userInCache || _kc.checkUserExisted(uid)))
//		return ErrorCode::USER_NOT_FOUND;
//	// request not found
//	if (!(requestInCache || _kc.checkRequestExisted(reqid)))
//		return ErrorCode::INVALID_PARAMETER;
//
//	// using request id to retrieve sender and receiver
//	string value;
//	FriendRequest tmp;
//	int id = requestId;
//	// cache
//	if (_cache.get<int32_t, FriendRequest>("friend.cache.friendRequest", id, tmp)) {
//		// found in cache
//	} else {
//		// not found in cache
//		// db
//		_kc.loadRequest(reqid, value);
//
//		if (value.size() == 0) return ErrorCode::INVALID_PARAMETER;
//
//		// convert from string to instance
//		_convert_req.deserialize(value, tmp);
//	}
//
//	if (to_string(tmp.p_recv_req) != uid) return ErrorCode::INVALID_PARAMETER;
//
//	// async call
//	_kc.store(uid, reqid, SubKC::DB_TYPE::REMOVE_PENDING_REQUEST);
//	_kc.store(uid, to_string(tmp.p_send_req), SubKC::DB_TYPE::REMOVE_PENDING_USER);
//
//	// append friend list of both users
//	_kc.store(uid, to_string(tmp.p_send_req), SubKC::DB_TYPE::FRIEND);
//	_kc.store(to_string(tmp.p_send_req), uid, SubKC::DB_TYPE::FRIEND);

	return ErrorCode::SUCCESS;
}

// copy from above code except code that store to Friend List DB

ErrorCode::type HandlerFriendServices::declineRequest(const int32_t curId, const int32_t requestId) {
//	Application::instance().logger().information("Add friend request");
//
//	// check user existed
//	string uid = to_string(curId);
//	string reqid = to_string(requestId);
//	bool userInCache = _cache.check<string, User>("friend.cache.user", uid);
//	bool requestInCache = _cache.check<string, FriendRequest>("friend.cache.friendRequest", reqid);
//
//	// user not found
//	if (!(userInCache || _kc.checkUserExisted(uid)))
//		return ErrorCode::USER_NOT_FOUND;
//	// request not found
//	if (!(requestInCache || _kc.checkRequestExisted(reqid)))
//		return ErrorCode::INVALID_PARAMETER;
//
//	// using request id to retrieve sender and receiver
//	string value;
//	FriendRequest tmp;
//
//	// cache
//	if (_cache.get<string, FriendRequest>("friend.cache.friendRequest", reqid, tmp)) {
//		// found in cache
//	} else {
//		// not found in cache
//		// db
//		_kc.loadRequest(reqid, value);
//
//		if (value.size() == 0) return ErrorCode::INVALID_PARAMETER;
//
//		// convert from string to instance
//		_convert_req.deserialize(value, tmp);
//	}
//
//	if (to_string(tmp.p_recv_req) != uid) return ErrorCode::INVALID_PARAMETER;
//
//	// async call
//	_kc.store(uid, reqid, SubKC::DB_TYPE::REMOVE_PENDING_REQUEST);
//	_kc.store(uid, to_string(tmp.p_send_req), SubKC::DB_TYPE::REMOVE_PENDING_USER);

	return ErrorCode::SUCCESS;
}

ErrorCode::type HandlerFriendServices::removeFriend(const int32_t curId, const int32_t friendId) {
	return ErrorCode::SUCCESS;
}

void HandlerFriendServices::viewFriendList(listFriendResult& _return, const int32_t id, const int32_t index, const int32_t size) {
//	Application::instance().logger().information("Incoming view friend list request");
//
//	try {
//		// check user existed
//		string uid = to_string(id);
//		if (!_kc.checkUserExisted(uid)) {
//			_return.code = ErrorCode::USER_NOT_FOUND;
//			return;
//		}
//
//		// get data
//		std::set<int32_t> tmp;
//		_kc.loadFriendList(uid, tmp);
//
//		// filter out of range starting index
//		if (index >= tmp.size() || index < 0) {
//			_return.code = ErrorCode::OUT_OF_RANGE;
//			return;
//		}
//
//		std::set<int32_t>::iterator it = tmp.begin();
//		for (int i = 0; i < index; i++)
//			it++;
//
//		// filter out of range data 
//		int limit_ = -1;
//		if (index + size >= tmp.size()){ // out of range or read all
//			limit_ = tmp.size();
//			_return.idx = -1; // no more data to load
//		} else {
//			limit_ = index + size;
//			_return.idx = limit_;
//		}
//		
//		for (int i = index; i < limit_; i++) {
//			User detailData;
//			FriendData simplifyData;
//			int32_t id = *(it++);
//			string id_str = to_string(id);
//			if (_cache.get<string, User>("friend.cache.user", id_str, detailData)) {
//				// found in cache
//			} else {
//				// not found
//				string value;
//				_kc.loadUser(id_str, value);
//				_convert_user.deserialize(value, detailData);
//			}
//			simplifyData.id = detailData.id;
//			simplifyData.name = detailData.name;
//			simplifyData.isMale = detailData.isMale;
//			_return.friendList.push_back(simplifyData);
//		}
//
//		// because we can get size of result from result itself so we should
//		// return something more useful for later processing
//		// size of all data
//		_return.size = tmp.size();
//		_return.code = ErrorCode::SUCCESS;
//	} catch (...) {
//		Application::instance().logger().error("Get friend list fail");
//		_return.code = ErrorCode::INTERNAL_ERROR;
//	}

	return;
}