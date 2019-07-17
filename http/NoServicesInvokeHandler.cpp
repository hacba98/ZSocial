/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "./ZRequestHandler.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

//////////////////////////////////////

void NoServicesInvokeHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &res) {
    string url = req.getURI();

    // check for cookie
    NameValueCollection nvc;
    req.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");

    // serve login - entry page
    if (url.empty() || url == "/") {
        bool flag = true;
        // if cookie has data
        if (!(uid == "no_cookies")) {
            url = "/dashboard";
            flag = false;
        }

        if (flag) {
            res.setStatus(HTTPResponse::HTTP_OK);
            res.setContentType("text/html");

            try {
                url = "./src/index.html";
                res.sendFile(url, "text/html");
            } catch (Exception e) {
                cout << e.message() << endl;
            }
        }
    }

    // serve register page
    if (url.find("/register") == 0) {
        res.setStatus(HTTPResponse::HTTP_OK);
        res.setContentType("text/html");

        try {
            url = "./src/register.html";
            res.sendFile(url, "text/html");
        } catch (Exception e) {
            cout << e.message() << endl;
        }
    }

    // serve dashboard
    if (url.find("/dashboard") == 0) {
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/html");

        try {
            
            string result,feed = "<p></p>",fr="%s";
            
            std::ostream& ostr = res.send();
            Poco::format(result,ZRequestHandlerFactory::dashboardString,feed,fr);
            ostr << result;
        } catch (Exception e) {
            Poco::Util::Application::instance().logger().error(e.message());
        }
        return;
    }
    if (url.find("/feed") == 0){
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/html");

        try {
            // retrieve list friend's newsfeed
            FeedCountResult feedRet;
            ListFeedResult listFeed;
            NewsFeedConnection *feedConn;
            while (!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
            feedConn->client()->getFeedCount(feedRet, atoi(uid.c_str()));
            feedConn->client()->getListFeed(listFeed,atoi(uid.c_str()),feedRet.result,5);
            
            // user data
            ProfileConnection *profileConn;
            GetUserResult userRet;
            while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));
            profileConn->client()->GetProfile(userRet,atoi(uid.c_str()));
            
            string result,feedString;
            if (listFeed.exitCode == 0){
                for(auto i = listFeed.result.feedlist.begin(); i != listFeed.result.feedlist.end(); ++i){
                    int d,m,y;
                    TOOL::getDMY(i->edit_time,d,m,y);
                    string date = Poco::NumberFormatter::format(y)+"-"+Poco::NumberFormatter::format0(m,2) + "-" + Poco::NumberFormatter::format0(d,2);
                    string feed ="<p text-align : right>" + userRet.profile.name + " - Date:" + date + "</p>\n" 
                            +"<p text-align : left>" + i->content + "</p> <hr>\n";
                    feedString.append(feed);
                }
            }
            Poco::format(result,ZRequestHandlerFactory::myfeedString,feedString);
            std::ostream& ostr = res.send();
            ostr << result;
        } catch (Exception e) {
            Poco::Util::Application::instance().logger().error(e.message());
        }
        return;
    }
    if (url.find("/myprofile") == 0) {
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/html");

        try {
            GetUserResult ret;
            ProfileConnection *profileConn;
            while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100))); // timeout 100 miliseconds
            profileConn->client()->GetProfile(ret,atoi(uid.c_str()));
            
            int d,m,y;
            TOOL::getDMY(ret.profile.birth,d,m,y);
            string result,date,gender = ret.profile.gender?"Nam":"Nu";
            date = Poco::NumberFormatter::format(y)+"-"+Poco::NumberFormatter::format0(m,2) + "-" + Poco::NumberFormatter::format0(d,2);
            
            Poco::format(result,ZRequestHandlerFactory::profileString,ret.profile.username,ret.profile.name,gender,ret.profile.phoneNumber,date);
            
            std::ostream& ostr = res.send();
            ostr << result;
        } catch (Exception e) {
            Poco::Util::Application::instance().logger().error(e.message());
        }
        return;
    }
    // serve add friend page
    if (url == "/friend") {
        res.setStatus(HTTPResponse::HTTP_OK);
        res.setContentType("text/html");

        try {
            url = "./src/friend.html";
            res.sendFile(url, "text/html");
        } catch (Exception e) {
            cout << e.message() << endl;
        }
    }
    
}
