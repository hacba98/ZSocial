/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ZRequestHandler.h"

//////////////////////////////////////
void NewsFeedRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
    std::string url = req.getURI();
    if (url.find("/newsFeed/create") == 0)
        return handleCreateRequest(req, res);
    else if (url.find("/newsFeed/update") == 0)
        return handleUpdateRequest(req, res);
    else if (url.find("/newsFeed/delete") == 0)
        return handleDeleteRequest(req, res);
    else if (url.find("/newsFeed/loadmore-myfeed") == 0)
        return handleLoadMoreRequest_MyFeed(req, res);
    else if (url.find("/newsFeed/loadmore-mywall") == 0)
        return handleLoadMoreRequest_MyWall(req, res);
    else{
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
    SimpleProfile token_;
    bool valid = ZRequestHandlerFactory::validCookie(token_, uid);
    
    if (!valid) // redirect to /login
	    return res.redirect("/login");
    
    int id = token_.id;
    
    FeedCreateResult ret;
    _conn->client()->createNewsFeed(ret,id,content,0);//status is 0 because no IDEA what it do
    if (ret.exitCode == 0){
        res.setStatus(HTTPResponse::HTTP_OK);
        ZRequestHandlerFactory::onClientPostFeed(id);
    }else{
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    }
    res.redirect("/feed");
};

void NewsFeedRequestHandler::handleUpdateRequest(Poco::Net::HTTPServerRequest &req,Poco::Net::HTTPServerResponse &res){
    Poco::Util::Application::instance().logger().information("UPDATE FEED Request from " + req.clientAddress().toString());
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    string content = form.get("content");
    int f_id = atoi(form.get("id").c_str());
    
    FeedUpdateResult ret;
    _conn->client()->updateNewsFeed(ret,f_id,content,0);//status is 0 because no IDEA what it do
    if (ret.exitCode == 0){
        res.setStatus(HTTPResponse::HTTP_OK);
        res.set("valid", "true");
    }else{
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        res.set("valid", "false");
    }
    res.send().flush();
};
void NewsFeedRequestHandler::handleDeleteRequest(Poco::Net::HTTPServerRequest &req,Poco::Net::HTTPServerResponse &res){
    Poco::Util::Application::instance().logger().information("Delete FEED Request from " + req.clientAddress().toString());
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    int f_id = atoi(form.get("id").c_str());
    
    NameValueCollection nvc;
    req.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");
    
    // token handle
    SimpleProfile token_;
    bool valid = ZRequestHandlerFactory::validCookie(token_, uid);
    
    if (!valid) // redirect to /login
	    return res.redirect("/login");
    
    int id = token_.id;
    
    FeedDeleteResult ret;
    _conn->client()->deleteNewsFeed(ret,f_id,id);
    if (ret.exitCode == 0){
        res.setStatus(HTTPResponse::HTTP_OK);
        res.set("valid", "true");
    }else{
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        res.set("valid", "false");
    }
    res.send().flush();
};

void NewsFeedRequestHandler::handleLoadMoreRequest_MyFeed(Poco::Net::HTTPServerRequest &req,Poco::Net::HTTPServerResponse &res){
    Poco::Util::Application::instance().logger().information("Load more owner FEED Request from " + req.clientAddress().toString());
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    Item item;
    item.id = atoi(form.get("next.id").c_str());
    item.post = atoi(form.get("next.post").c_str());
    
    res.setStatus(HTTPResponse::HTTP_OK);
    res.setContentType("text/html");
    
    if (item.id == -1 && item.post == -1) {
        res.set("next.id", std::to_string(-1));
        res.set("next.post", std::to_string(-1));
        res.set("data" , "<p> no more feed ... </p>");
        res.set("valid", "true");
        
        res.send().flush();
        return;
    }
    
    NameValueCollection nvc;
    req.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");
    
    // token handle
    SimpleProfile token_;
    bool valid = ZRequestHandlerFactory::validCookie(token_, uid);
    
    if (!valid) // redirect to /login
	    return res.redirect("/login");
    
    int id = token_.id;
    
    ListFeedResult ret;
    
    _conn->client()->getListFeed(ret,id,item,2);
    if (ret.exitCode == 0){
        string feedString;
        for (auto i = ret.result.feedlist.begin(); i != ret.result.feedlist.end(); ++i) {
            int d, m, y, hh, pp;
            TOOL::getDMY(i->edit_time, d, m, y, hh, pp);
            string date = Poco::NumberFormatter::format(y) + "-" + Poco::NumberFormatter::format0(m, 2) + "-" + Poco::NumberFormatter::format0(d, 2)
                    + "  " + Poco::NumberFormatter::format0(hh, 2) + ":" + Poco::NumberFormatter::format0(pp, 2);
            string feed = "<form class=\"card\" name=\"feed_no_" + std::to_string(i->id) + "\" onsubmit=\"updateFeed('" + std::to_string(i->id) + "')\">" +
                    "<p class=\"price\"> Date:" + date + "</p>" +
                    "<textarea class=\"status_post\" rows=\"4\" name=\"content\">" + i->content + "</textarea><br>" +
                    "<input type =\"button\" class=\"updateBTN\" onClick=\"updateFeed('" + std::to_string(i->id) + "')\" value=\"UPDATE\">" +
                    "<input type =\"button\" class=\"deleteBTN\" onClick=\"removeFeed('" + std::to_string(i->id) + "')\" value=\"REMOVE\">" +
                    "</form> <hr><br><br>";
            feedString.append(feed);
        }
        res.set("next.id", std::to_string(ret.result.nex.id));
        res.set("next.post", std::to_string(ret.result.nex.post));
        res.set("data" , feedString);
        res.set("valid", "true");
    }else{
        res.set("valid", "false");
    }
    res.send().flush();
};

void NewsFeedRequestHandler::handleLoadMoreRequest_MyWall(Poco::Net::HTTPServerRequest &req,Poco::Net::HTTPServerResponse &res){
    Poco::Util::Application::instance().logger().information("Load more wall FEED Request from " + req.clientAddress().toString());
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    Item item;
    item.id = atoi(form.get("next.id").c_str());
    item.post = atoi(form.get("next.post").c_str());
    
    res.setStatus(HTTPResponse::HTTP_OK);
    res.setContentType("text/html");
    
    if (item.id == -1 && item.post == -1) {
        res.set("next.id", std::to_string(-1));
        res.set("next.post", std::to_string(-1));
        res.set("data" , "<p> no more feed ... </p>");
        res.set("valid", "true");
      
        res.send().flush();
        return;
    }
    
    NameValueCollection nvc;
    req.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");
    
    // token handle
    SimpleProfile token_;
    bool valid = ZRequestHandlerFactory::validCookie(token_, uid);
    
    if (!valid) // redirect to /login
	    return res.redirect("/login");
    
    int id = token_.id;
    
    ListFeedResult ret;
    
    _conn->client()->getListWall(ret,id,item,2);
    if (ret.exitCode == 0){
        ProfileConnection *profileConn;
	while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));
        
        string feedString;
        
        for (auto i = ret.result.feedlist.begin(); i != ret.result.feedlist.end(); ++i) {
            if (i->edit_time == 0)
		continue;
            int d, m, y, hh, pp;
            TOOL::getDMY(i->edit_time, d, m, y, hh, pp);
                GetUserResult userRet;
                profileConn->client()->GetProfile(userRet,i->owner);
            string date = Poco::NumberFormatter::format(y) + "-" + Poco::NumberFormatter::format0(m, 2) + "-" + Poco::NumberFormatter::format0(d, 2)
                    + "  " + Poco::NumberFormatter::format0(hh, 2) + ":" + Poco::NumberFormatter::format0(pp, 2);
                
                string feed = "<div class=\"card\"> <h1>" + userRet.profile.name + "</h1>" +
                        "<p class=\"price\">" + date + "</p>" +
                            "<p>" + i->content + "</p><p><button>Like</button></p></div><br><br>";
                for(auto it = feed.begin() ; it != feed.end() ; ++it){
                    if (*it == '\n' || *it == '\r') *it = '-';
                }
            feedString.append(feed);
        }
        ZRequestHandlerFactory::profilePool()->returnObject(profileConn);
        res.set("next.id", std::to_string(ret.result.nex.id));
        res.set("next.post", std::to_string(ret.result.nex.post));
        res.set("data" , feedString);
        res.set("valid", "true");
    }else{
        res.set("valid", "false");
    }
    res.send().flush();
};
