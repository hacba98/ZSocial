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
boost::shared_ptr<Poco::ObjectPool<NewsFeedConnection> > ZRequestHandlerFactory::_pool_friends;

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
	} else if (url == "/newsfeed"){
		NewsFeedConnection *borrowObj;
		while(!(borrowObj = newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
		return new FriendRequestHandler(borrowObj);
	} else {
		return new NoServicesInvokeHandler;
	}
}

//////////////////////////////////////

void ProfileRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& res){
	Application::instance().logger().information("line 76");
        std::string url = req.getURI();
	if (url.find("/profile/login/") == 0)
		return handleLogin(req, res);
        else if (url.find("/profile/register") == 0){
            
            return handleRegister(req,res);
        }
	
}

void ProfileRequestHandler::handleRegister(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res){
    Application& app = Application::instance();
    app.logger().information("CREATE Request from " + req.clientAddress().toString());

    HTMLForm form(req, req.stream());

    string name = form.get("name", "No one will name like this");
    string gender = form.get("gender", "");
    
    long phoneNumber = (long)atoi(form.get("phonenumber", "-1").c_str());
    string username =  form.get("username","");
    string password =  form.get("password","");
    string repassword =form.get("repassword","");
    bool ok = (name != "No one will name like this") && (gender != "") && (phoneNumber != -1l) &&
                (username != "") && (password != "") && (repassword == password);
    
    Poco::DateTime birthDate;
    int tz_diff;
    Poco::DateTimeParser::parse("yyyy-mm-dd", form.get("birth", "2000-01-01"), birthDate, tz_diff);
    int birth = (int)(birthDate.timestamp().epochTime());
    app.logger().information(birth);
    UserProfile profile;
    TOOL::setProfile(profile , name,gender ,birth,phoneNumber,username ,password);

    CreateUserResult ret;
    if (ok) {
        _conn->client()->CreateProfile(ret, profile);
    }

    if (ret.errorCode == ErrorCode::SUCCESS && ok){ // valid case need to response token
		res.setStatus(HTTPResponse::HTTP_OK);
		res.set("valid", "true");
	} else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	
	res.send().flush();
};

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
