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

void NoServicesInvokeHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &res){
	string url = req.getURI();
	
	if (url.empty() || url == "/"){
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/html");
		
		try {
			url = "./src/index.html";
			res.sendFile(url, "text/html");
		} catch (Exception e){
			cout << e.message() << endl;
		}
	}
}

//////////////////////////////////////
void ProfileRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& res){
	Application::instance().logger().information(req.getURI());
}