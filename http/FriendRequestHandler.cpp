/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZRequestHandler.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;

void FriendRequestHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& res) {
	// retrieve data from request
	string url = req.getURI();
	NameValueCollection cookies;
	req.getCookies(cookies);
	string uid = cookies.get("zuid", "no_cookies");
	
	// case that dont have cookie -> guard - force to login again
	if (uid == "no_cookies"){
		return res.redirect("/login");
	}
	
	// serve load friend page
	if (req.getMethod() == "GET" && (url == "/friend/page" || url == "/friend/" || url == "/friend")){
		return handleLoadPage(req, res, uid);
	}
	
	// serve add friend request
	if (req.getMethod() == "POST" && url == "/friend/add"){
		return handleAddFriend(req, res, uid);
	}
	
	// serve css file
	if (url == "/friend/friend.css" || url == "/friend/friend/friend.css"){
		res.setStatus(HTTPResponse::HTTP_OK);
		res.sendFile("./src/friend.css", "text/css");
		return;
	}
}

/**
 * GET API /friend/page && /friend/
 * @param req
 * @param res
 * @param uid - userID getting from cookies
 */
void FriendRequestHandler::handleLoadPage(HTTPServerRequest& req, HTTPServerResponse& res, string uid){
	string pendingHTMLCode; // code for render pending list
	string friendListHTMLCode = "<p> TODO </p>"; // code for render friend list
	string htmlCode; // code for return to render html page
	try {
		// connect to friend service
		int user_id = stoi(uid);
		
		// check for pending requests
		pingResult ret;
		_conn->client()->checkRequest(ret, user_id);
		
		if (!(ret.code == ErrorCode::SUCCESS)){
			res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
			return;
		}
		
		if(ret.data.size() == 0) {
			pendingHTMLCode = "<p> You don't have any friend request. <p>";
		} else {
			char buf[50];
			snprintf(buf, 50, "You have %lu following request(s):", ret.data.size());
			pendingHTMLCode.append(buf);
			for (int i=0; i < ret.data.size(); i++){
				FriendRequest freq = ret.data[i];
				ostringstream oss;
				std::time_t t_ptr = static_cast<std::time_t> (freq.time);
				oss << "<form action=\"/acceptRequest\" method=\"GET\">";
				oss << "<input type=\"hidden\" name=\"index\" value=\"" << i << "\"/>";
				oss << "<label> Request Id: " << freq.id
					<< ". User Id: " << freq.p_send_req
					<< ". Message: " << freq.greeting
					<< ". Time: " << std::ctime(&t_ptr) << "</label>";
				oss << "<input type=\"Submit\" value=\"Add\">";
				oss << "</form>";
	
				string eachRequestCode = oss.str();
				pendingHTMLCode.append(eachRequestCode);
			}
		}
		
		// TODO - add code for friend list
		
		// read HTML file as string 
		Poco::FileInputStream htmlFile("./src/friend.html");
		string code;
		while (!htmlFile.eof()){
			char line[256];
			//htmlFile >> line;
			htmlFile.getline(line, 256);
			code.append(line);
			code.append("\n");
		}
		
		// put render code into template string
		Poco::format(htmlCode, code, pendingHTMLCode, friendListHTMLCode);
		cout << htmlCode;
//		char buf[512];
//		snprintf(buf, htmlCode.size(), htmlCode.c_str(), 
//			pendingHTMLCode.c_str(), friendListHTMLCode.c_str());
		
		// response to client
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setChunkedTransferEncoding(true);
                res.setContentType("text/html");
		ostream& out = res.send();
		out << htmlCode;
		out.flush();
	} catch(...){
		Application::instance().logger().error("Error-Load Friend Page Error");
		return res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
	}
}

/**
 * POST API: /friend/add
 * @param req
 * @param res
 * @param uid - userID getting from cookies
 */
void FriendRequestHandler::handleAddFriend(HTTPServerRequest& req, HTTPServerResponse& res, string uid){
	try {
		istream& body_stream = req.stream();
		HTMLForm form(req, body_stream);
		string fr_username = form.get("friend_username", "no_user_name");
		string greeting = form.get("message");

		// Mapping username to userid of friend
		ProfileConnection *borrow;
		while (!(borrow = ZRequestHandlerFactory::profilePool()->borrowObject(1000))); // wait 1 sec
		int fr_id = borrow->client()->GetIdByName(fr_username);
		ZRequestHandlerFactory::profilePool()->returnObject(borrow);

		// check friend ID existed
		if (fr_id == -1){ // not found
			string reason = "Username not found";
			res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
			//res.setStatusAndReason(HTTPResponse::HTTP_NOT_FOUND, reason);
			return;
		}
		
		// Create a new friend request
		FriendRequest request;
		request.p_send_req = stoi(uid);
		request.p_recv_req = fr_id;
		request.greeting = greeting;
		
		// calling async api
		_conn->client()->addFriend(request);
		res.setStatus(HTTPResponse::HTTP_OK);
	} catch (...){ // some thing wrong
		Application::instance().logger().error("Error-Add Friend Request");
		return res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
	}
}