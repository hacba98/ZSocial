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

void NoServicesInvokeHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &res){
	string url = req.getURI();
	
	// serve login - entry page
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
	
	// serve register page
	if (url == "/register"){
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/html");
		
		try {
			url = "./src/register.html";
			res.sendFile(url, "text/html");
		} catch (Exception e){
			cout << e.message() << endl;
		}
	}
	
	// serve dashboard
	if (url.find("/dashboard") == 0){
		//res.setStatus(HTTPResponse::HTTP_OK);
		res.setChunkedTransferEncoding(true);
                res.setContentType("text/html");
		
		try {
			Poco::FileInputStream htmlFile("./src/dashboard.html");
			string line,dashboardString;
			while(!htmlFile.eof()) {
                            htmlFile >> line;
                            dashboardString.append(line+"\n");
                        }
			//Poco::Util::Application::instance().logger().information(dashboardString);
			// retrieve list friend's newsfeed
			FeedResult feedRet;
			NewsFeedConnection *feedConn;
			while(!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
			feedConn->client()->getFeed(feedRet, 1);
                        
                        std::ostream& ostr = res.send();
			ostr << dashboardString;
		} catch (Exception e){
			Poco::Util::Application::instance().logger().error(e.message());
		}
	}
	
	// serve add friend page
	if (url == "/friend"){
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/html");
		
		try {
			url = "./src/friend.html";
			res.sendFile(url, "text/html");
		} catch (Exception e){
			cout << e.message() << endl;
		}
	}
}