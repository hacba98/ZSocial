/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ZRequestHandler.cpp
 * Author: cpu10855
 * 
 * Created on July 4, 2019, 10:04 AM
 */

#include "ZRequestHandler.h"

boost::shared_ptr<Poco::ObjectPool<ProfileConnection> > ZRequestHandlerFactory::_pool_profiles;
boost::shared_ptr<Poco::ObjectPool<FriendConnection> > ZRequestHandlerFactory::_pool_friends;
boost::shared_ptr<Poco::ObjectPool<NewsFeedConnection> > ZRequestHandlerFactory::_pool_newsfeed;
boost::shared_ptr<Poco::ObjectPool<Converter<token> > > ZRequestHandlerFactory::_pool_convert_token;

std::string ZRequestHandlerFactory::_secret;
map<int, Poco::Net::WebSocket*> ZRequestHandlerFactory::_clients;


using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;
using namespace Poco::Crypto;

string ZRequestHandlerFactory::dashboardString;
string ZRequestHandlerFactory::loginString;
string ZRequestHandlerFactory::registerString;
string ZRequestHandlerFactory::profileString;
string ZRequestHandlerFactory::friendString;
string ZRequestHandlerFactory::myfeedString;

Poco::Net::HTTPRequestHandler * ZRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &req) {
    Application::instance().logger().information(req.getURI());
	std::string url = req.getURI();

	if (url.find("/profile") == 0) {
		ProfileConnection *borrowObj;
		while (!(borrowObj = profilePool()->borrowObject(100))); // timeout 100 miliseconds
		return new ProfileRequestHandler(borrowObj);
	} else if (url.find("/friend") == 0) {
		FriendConnection *borrowObj;
		while (!(borrowObj = friendPool()->borrowObject(100))); // timeout 100 miliseconds
		return new FriendRequestHandler(borrowObj);
	} else if (url.find("/newsFeed") == 0) {
		NewsFeedConnection *borrowObj;
		while (!(borrowObj = newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
		return new NewsFeedRequestHandler(borrowObj);
	} else {
		// handle socket connection
		if (req.find("Upgrade") != req.end() && Poco::icompare(req["Upgrade"], "websocket") == 0) {
			return new WebSocketHandler;
		}

		// if not web-socket -> return page load handler
		return new NoServicesInvokeHandler;
	}
}

string ZRequestHandlerFactory::genCookie(int zuid){
	// create new thrift token object and set required data
	token token_;
	token_.zuid = zuid;
	
	// convert that to string
	//Converter<token> borrowObj;
	Converter<token> *borrowObj;
	while (!(borrowObj = converterPool()->borrowObject(100))); // timeout 100 miliseconds
	
	string serialized_str;
	borrowObj->serialize(token_, serialized_str);
	converterPool()->returnObject(borrowObj);
	
	// need to turn string from serialized obj to base64 encode for transferable 
	stringstream ss;
	Poco::Base64Encoder b64encode(ss);
	b64encode << serialized_str;
	b64encode.close(); // must have this for encoder to complete the processing
	string payload = ss.str();
	
	// hash the string result with secret running variable of server to get signature
	Poco::HMACEngine<SHA1Engine> hmac(ZRequestHandlerFactory::_secret);
	hmac.update(payload);
	string signature = Poco::DigestEngine::digestToHex(hmac.digest()); //string 
	
	// append payload and signature to have token
	return (payload + "." + signature);
}

bool ZRequestHandlerFactory::validCookie(token& token_, std::string cookie){
	// read the payload part and signature part from token string
	string delimiter = ".";
	string payload = cookie.substr(0, cookie.find(delimiter));
	string signature = cookie.substr(cookie.find(delimiter) + delimiter.length(), cookie.length());
	
	// check if signature is valid
	Poco::HMACEngine<SHA1Engine> hmac(ZRequestHandlerFactory::_secret);
	hmac.update(payload);
	if (!(signature == Poco::DigestEngine::digestToHex(hmac.digest()))){
		// signature is not valid -> return false immediately
		return false;
	}
	
	// signature is correct -> get data from payload and put into token
	stringstream iss, oss;
	iss << payload;
	Poco::Base64Decoder b64decode(iss);
	copy(std::istreambuf_iterator<char>(b64decode),
		std::istreambuf_iterator<char>(),
		std::ostreambuf_iterator<char>(oss));
	string serialized_str = oss.str();

	// invoke converter to deserialize string back to thrift object
	token tmp;
	//Converter<token> borrowObj;
	Converter<token> *borrowObj;
	while (!(borrowObj = converterPool()->borrowObject(100))); // timeout 100 miliseconds
	
	try {
		borrowObj->deserialize(serialized_str, tmp);
		token_ = tmp;
	} catch (...){
		Application::instance().logger().information("Debug");
	}
	
	converterPool()->returnObject(borrowObj);
	
	// return true for token validity
	return true;
}

void ZRequestHandlerFactory::onClientConnect(int zuid) {
	// retrieve user friend list
	listFriendResult friends;
	FriendConnection *borrowObj;
	while (!(borrowObj = ZRequestHandlerFactory::friendPool()->borrowObject(100)));
	borrowObj->client()->viewFriendList(friends, zuid, 0, 100); // get 100 friends - TODO: get all
	
	// loop for friend list, if they are connected to server
	// then send them a notification
	if (friends.code == ErrorCode::SUCCESS){
		WebSocket *ws;
		for (int i=0; i < friends.friendList.size(); i++){
			int friend_id = friends.friendList.at(i);
			map<int, WebSocket*>::iterator it = clients()->find(friend_id);
			if (it != clients()->end() && it->first == friend_id) {
				ws = it->second;
				string payload = "online:" + to_string(zuid);
				ws->sendFrame(payload.c_str(), payload.size(), WebSocket::FRAME_TEXT);
				Application::instance().logger().information(payload);
			}
		}
	}
}

void ZRequestHandlerFactory::onClientDisconnect(int zuid){
	// retrieve user friend list
	listFriendResult friends;
	FriendConnection *borrowObj;
	while (!(borrowObj = ZRequestHandlerFactory::friendPool()->borrowObject(100)));
	borrowObj->client()->viewFriendList(friends, zuid, 0, 100); // get 100 friends - TODO: get all
	
	// loop for friend list, if they are connected to server
	// then send them a notification
	if (friends.code == ErrorCode::SUCCESS){
		WebSocket *ws;
		for (int i=0; i < friends.friendList.size(); i++){
			int friend_id = friends.friendList.at(i);
			map<int, WebSocket*>::iterator it = clients()->find(friend_id);
			if (it != clients()->end() && it->first == friend_id) {
				ws = it->second;
				string payload = "offline:" + to_string(zuid);
				ws->sendFrame(payload.c_str(), payload.size(), WebSocket::FRAME_TEXT);
				Application::instance().logger().information(payload);
			}
		}
	}
}
