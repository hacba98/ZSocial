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

boost::shared_ptr<Poco::ObjectPool<FriendConnection> > ZRequestHandlerFactory::_pool_friends;
boost::shared_ptr<Poco::ObjectPool<NewsFeedConnection> > ZRequestHandlerFactory::_pool_newsfeed;

Poco::Net::HTTPRequestHandler * ZRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &req) {
    Application::instance().logger().information(req.getURI());
    std::string url = req.getURI();

    if (url.find("/profile") == 0) {
        ProfileConnection *borrowObj;
        while (!(borrowObj = profilePool()->borrowObject(100))); // timeout 100 miliseconds
        return new ProfileRequestHandler(borrowObj);
    } else if (url == "/friend") {
        FriendConnection *borrowObj;
        while (!(borrowObj = friendPool()->borrowObject(100))); // timeout 100 miliseconds
        return new FriendRequestHandler(borrowObj);
    } else if (url.find("/newsFeed") == 0) {
        NewsFeedConnection *borrowObj;
        while (!(borrowObj = newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
        return new NewsFeedRequestHandler(borrowObj);
    } else {
        return new NoServicesInvokeHandler;
    }
}

//////////////////////////////////////
void NewsFeedRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
    std::string url = req.getURI();
    if (url.find("/newsFeed/create") == 0)
        return handleCreateRequest(req, res);
    else  {
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        res.send().flush();
    }
};

void NewsFeedRequestHandler::handleCreateRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
    Poco::Util::Application::instance().logger().information("ADD FEED Request from " + req.clientAddress().toString());
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    string content = form.get("content");
    
    NameValueCollection nvc;
    req.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");
    int id = atoi(uid.c_str());
    
    FeedCreateResult ret;
    _conn->client()->createNewsFeed(ret,id,content,0);//status is 0 because no IDEA what it do
    if (ret.exitCode == 0){
        res.setStatus(HTTPResponse::HTTP_OK);
    }else{
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    }
    res.send().flush();
};
