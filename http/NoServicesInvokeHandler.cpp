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
	
	// serve icon request from browser
	if (url == "/favicon.ico"){
		url = "./src/icon.png";
		res.sendFile(url, "image/png");
		return;
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
        return;
    }

    // Guard
    if (uid == "no_cookies")
        url = "/";

    // serve login - entry page
    if (url.empty() || url == "/" || url == "/login") {
        bool flag = true;
        // if cookie has data
        if (!(uid == "no_cookies")) {
            res.redirect("/dashboard");
            return;
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
            return;
        }
    }
    if (url.find("/dashboard") == 0) {
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/html");

        try {

            string result, feed = "%s", fr = "%s";

            std::ostream& ostr = res.send();
            Poco::format(result, ZRequestHandlerFactory::dashboardString, feed, fr);
            ostr << result;
        } catch (Exception e) {
            Poco::Util::Application::instance().logger().error(e.message());
        }
        return;
    }
    if (url.find("/feed") == 0) {
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/html");

        try {
            // retrieve list friend's newsfeed
            int user_id = ZRequestHandlerFactory::getUIDfromCookie(uid);
            FeedCountResult feedRet;
            ListFeedResult listFeed;
            NewsFeedConnection *feedConn;
            while (!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
            feedConn->client()->getFeedCount(feedRet, user_id);
            feedConn->client()->getListFeed(listFeed, user_id, feedRet.result, 5);

            // user data
            ProfileConnection *profileConn;
            GetUserResult userRet;
            while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));
            profileConn->client()->GetProfile(userRet, user_id);

            string result, feedString;
            if (listFeed.exitCode == 0) {
                for (auto i = listFeed.result.feedlist.begin(); i != listFeed.result.feedlist.end(); ++i) {
                    int d, m, y;
                    TOOL::getDMY(i->edit_time, d, m, y);
                    string date = Poco::NumberFormatter::format(y) + "-" + Poco::NumberFormatter::format0(m, 2) + "-" + Poco::NumberFormatter::format0(d, 2);
                    string feed = "<form name=\"feed_no_" + std::to_string(i->id) + "\" onsubmit=\"updateFeed('" + std::to_string(i->id) + "')\">\n" +
                            "<p name=\"feed_id\" hidden>" + std::to_string(i->id) + "</p>\n" +
                            "<p>" + userRet.profile.name + " - Date:" + date + "</p>\n" +
                            "<textarea class=\"status_post\" rows=\"4\" name=\"content\">" + i->content + "</textarea><br>\n" +
                            "<input type =\"button\" class=\"post_button\" onClick=\"updateFeed('" + std::to_string(i->id) + "')\" value=\"UPDATE\">\n" +
                            "</form> <hr>\n";
                    feedString.append(feed);
                }
            }
            Poco::format(result, ZRequestHandlerFactory::myfeedString, feedString);
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
            profileConn->client()->GetProfile(ret, ZRequestHandlerFactory::getUIDfromCookie(uid));

            int d, m, y;
            TOOL::getDMY(ret.profile.birth, d, m, y);
            string result, date, gender = ret.profile.gender ? "Nam" : "Nu";
            date = Poco::NumberFormatter::format(y) + "-" + Poco::NumberFormatter::format0(m, 2) + "-" + Poco::NumberFormatter::format0(d, 2);

            Poco::format(result, ZRequestHandlerFactory::profileString, ret.profile.username, ret.profile.name, gender, ret.profile.phoneNumber, date);

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

    // serve unknow path
    res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    return;
}

//void NoServicesInvokeHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &res){
//	string url = req.getURI();
//	
//	// check for cookie
//	NameValueCollection nvc;
//	req.getCookies(nvc);
//	string uid = nvc.get("zuid", "no_cookies");
//	
//	// serve login - entry page
//	if (url.empty() || url == "/" || url == "/login"){
//		bool flag = true;
//		// if cookie has data
//		if (!(uid == "no_cookies")){
//			url = "/dashboard";
//			flag = false;
//		}
//		
//		if (flag){
//			res.setStatus(HTTPResponse::HTTP_OK);
//			res.setContentType("text/html");
//
//			try {
//				url = "./src/index.html";
//				res.sendFile(url, "text/html");
//			} catch (Exception e) {
//				cout << e.message() << endl;
//			}
//			return;
//		}
//	} 
//	
//	// serve register page
//	if (url == "/register"){
//		res.setStatus(HTTPResponse::HTTP_OK);
//		res.setContentType("text/html");
//		
//		try {
//			url = "./src/register.html";
//			res.sendFile(url, "text/html");
//		} catch (Exception e){
//			cout << e.message() << endl;
//		}
//		return;
//	}
//	
//	
//	
//	// serve dashboard
//	if (url.find("/dashboard") == 0){
//		//res.setStatus(HTTPResponse::HTTP_OK);
//		res.setChunkedTransferEncoding(true);
//                res.setContentType("text/html");
//		
//		try {
//			Poco::FileInputStream htmlFile("./src/dashboard.html");
//			string line,dashboardString;
//			while(!htmlFile.eof()) {
//                            htmlFile >> line;
//                            dashboardString.append(line+"\n");
//                        }
//			//Poco::Util::Application::instance().logger().information(dashboardString);
//			// retrieve list friend's newsfeed
//			FeedResult feedRet;
//			NewsFeedConnection *feedConn;
//			while(!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
//			feedConn->client()->getFeed(feedRet, atoi(uid.c_str()));
//                        
//                        
//                        std::ostream& ostr = res.send();
//			ostr << dashboardString;
//		} catch (Exception e){
//			Poco::Util::Application::instance().logger().error(e.message());
//		}
//		return;
//	}
//	
//	// serve add friend page
//	if (url == "/friend"){
//		res.setStatus(HTTPResponse::HTTP_OK);
//		res.setContentType("text/html");
//		
//		try {
//			url = "./src/friend.html";
//			res.sendFile(url, "text/html");
//		} catch (Exception e){
//			cout << e.message() << endl;
//		}
//		return;
//	}
//	
//	
//}
