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

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

Poco::Net::HTTPRequestHandler * ZRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &req){
	Application::instance().logger().information(req.getURI());
	std::string url = req.getURI();
		
	if (url.find("/profile") == 0 ){
		ProfileConnection *borrowObj;
		while(!(borrowObj = profilePool()->borrowObject(100))); // timeout 100 miliseconds
		return new ProfileRequestHandler(borrowObj);
	} else if (url == "/friend"){
		FriendConnection *borrowObj;
		while(!(borrowObj = friendPool()->borrowObject(100))); // timeout 100 miliseconds
		return new FriendRequestHandler(borrowObj);
	} else {
		return new NoServicesInvokeHandler;
	}
}


//////////////////////////////////////

void ProfileRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& res){
	std::string url = req.getURI();
	if (url.find("/profile/login/") == 0)
		return handleLogin(req, res);
	
}

void ProfileRequestHandler::handleLogin(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& res){
	// parsing data: username and password
	istream& body_stream = req.stream();
	HTMLForm form(req, body_stream);
	string username = form.get("username");
	string password = form.get("password");
	
	// using given connection to process server bussiness
	loginResult loginRet;
	_conn->client()->Login(loginRet, username, password);
	
	if (loginRet.code == ErrorCode::SUCCESS){ // valid case need to response token
		res.setStatus(HTTPResponse::HTTP_OK);
		res.set("valid", "true");
	} else if (loginRet.code == ErrorCode::USER_NOT_FOUND){
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	
	res.send().flush();
}