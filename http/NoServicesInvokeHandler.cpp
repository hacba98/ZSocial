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
    if (url == "/register") {
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
            Poco::FileInputStream htmlFile("./src/dashboard.html");
            string line, dashboardString;
            while (!htmlFile.eof()) {
                htmlFile >> line;
                dashboardString.append(line + "\n");
            }

            // retrieve list friend's newsfeed
            FeedResult feedRet;
            NewsFeedConnection *feedConn;
            while (!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
            feedConn->client()->getFeed(feedRet, atoi(uid.c_str()));


            std::ostream& ostr = res.send();
            ostr << dashboardString;
        } catch (Exception e) {
            Poco::Util::Application::instance().logger().error(e.message());
        }
    }
    if (url == "/myprofile") {
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/html");

        try {
            Poco::FileInputStream htmlFile("./src/profile.html");
            
            string dashboardString;
            while (!htmlFile.eof()) {
                char line[255];
                htmlFile.getline(line ,255);
                dashboardString.append(line);
                dashboardString.append("\n");
            }
            
            GetUserResult ret;
            ProfileConnection *profileConn;
            while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100))); // timeout 100 miliseconds
            profileConn->client()->GetProfile(ret,atoi(uid.c_str()));
            
            int d,m,y;
            TOOL::getDMY(ret.profile.birth,d,m,y);
            string result,date,gender = ret.profile.gender?"Nam":"Nu";
            date = Poco::NumberFormatter::format(y)+"-"+Poco::NumberFormatter::format0(m,2) + "-" + Poco::NumberFormatter::format0(d,2);
            Poco::format(result,dashboardString,ret.profile.username,ret.profile.name,gender,ret.profile.phoneNumber,date);
            
            //Poco::Util::Application::instance().logger().information(result);
            
            std::ostream& ostr = res.send();
            ostr << result;
        } catch (Exception e) {
            Poco::Util::Application::instance().logger().error(e.message());
        }
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
