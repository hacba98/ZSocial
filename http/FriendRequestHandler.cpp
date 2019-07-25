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
	string paging_index = cookies.get("next_idx", "0");
	
	// case that dont have cookie -> guard - force to login again
	if (uid == "no_cookies"){
		return res.redirect("/login");
	}
	
	// valid cookie: false -> redirect login
	token token_;
	if (!ZRequestHandlerFactory::validCookie(token_, uid)) {
		return res.redirect("/login");
	}
	
	// serve load friend page first time
	if (req.getMethod() == "GET" && (url == "/friend/page" || url == "/friend/" || url == "/friend")){
		return handleLoadPage(req, res, token_.zuid, 0);
	}
	
	// serve load friend page with click next button in render friend list
	if (req.getMethod() == "GET" && (url == "/friend/next")){
		if (paging_index == "-1"){
			res.redirect("/friend");
			return;
		}
		return handleLoadPage(req, res, token_.zuid, stoi(paging_index));
	}
	
	// serve add friend request
	if (req.getMethod() == "POST" && url == "/friend/add"){
		return handleAddFriend(req, res, token_.zuid);
	} 
	
	// serve accept friend request
	if (req.getMethod() == "GET" && url.find("/friend/acceptRequest") == 0){
		return handleAcceptRequest(req, res, token_.zuid);
	}
	
	// serve loadmore data request
	if (req.getMethod() == "POST" && url.find("/friend/loadmore") == 0){
		return handleLoadmoreRequest(req, res, token_.zuid);
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
void FriendRequestHandler::handleLoadPage(HTTPServerRequest& req, HTTPServerResponse& res, int uid, int paging_index){
	string pendingHTMLCode; // code for render pending list
	string friendListHTMLCode; // code for render friend list
	string htmlCode; // code for return to render html page
//	NameValueCollection nvc;
//	req.getCookies(nvc);
	try {
		// connect to friend service
		// get id from session management
		int user_id = uid;
		
		// check for pending requests
		{
			pingResult ret;
			_conn->client()->checkRequest(ret, user_id);

			if (!(ret.code == ErrorCode::SUCCESS)) {
				res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
				return;
			}

			if (ret.data.size() == 0) {
				pendingHTMLCode = "<br><br><p> You don't have any friend request. <p><br><br>";
			} else {
				char buf[100];
				snprintf(buf, 100, "<br><p>You have %lu following request(s):</p><br><br>", ret.data.size());
				pendingHTMLCode.append(buf);
				for (int i = 0; i < ret.data.size(); i++) {
					FriendRequest freq = ret.data[i];
					ostringstream oss;
					std::time_t t_ptr = static_cast<std::time_t> (freq.time);
					oss << "<form action=\"/friend/acceptRequest\" method=\"GET\">";
					oss << "<input type=\"hidden\" name=\"friend.requestId\" value=\"" << freq.id << "\"/>";
					oss << "<label> Request Id: " << freq.id
						<< ". User Id: " << freq.p_send_req
						<< ". Message: " << freq.greeting
						<< ". Time: " << std::ctime(&t_ptr) << "</label>";
					oss << "<input type=\"Submit\" name=\"button_accept\" value=\"Accept\">";
					oss << "<input type=\"Submit\" name=\"button_decline\" value=\"Decline\">";
					oss << "</form>";

					string eachRequestCode = oss.str();
					pendingHTMLCode.append(eachRequestCode);
				}
			}
		}
		
		
		// render friend list
		{
			// retreive session cookie for paging current index
			// string paging_index = nvc.get("CURRENT_IDX", "0");
			
			listFriendResult ret;
			_conn->client()->viewFriendList(ret, user_id, paging_index, 1);
			
			if (ret.code == ErrorCode::SUCCESS){
				if (ret.size == 0){
					friendListHTMLCode = "<p> You dont have any friends. Try to make more. </p>";
				} else {
					ProfileConnection *borrowObj;
					while (!(borrowObj = ZRequestHandlerFactory::profilePool()->borrowObject(1000))); // timeout 1s
					// get detail friend data from profile services
					ListProfileResult list_profiles;
					borrowObj->client()->getList(list_profiles, ret.friendList);
					
					if (list_profiles.errorCode == ErrorCode::SUCCESS){
						char buf[100];
						snprintf(buf, 100, "<br><p>You have %d friend(s):</p><br>", ret.size);
						friendListHTMLCode.append(buf);
						
						// render each line of code for friend data
						for (int i=0; i < list_profiles.profiles.size(); i++){
							SimpleProfile profile = list_profiles.profiles[i];
							ostringstream oss;
							oss << "<p> Your friend's username: \"" <<  profile.name << "\"";
							oss << (profile.gender ? ". He " : ". She ");
							oss << (profile.last_active_time == -1 ? "is online." : "is offline.");
							oss << "</p><br>";
							
							string eachFriendCode = oss.str();
							friendListHTMLCode.append(eachFriendCode);
						}
						
						// set session cookie for paging, it will store next index of friend list
						HTTPCookie paging_cookie;
						paging_cookie.setMaxAge(-1);
						paging_cookie.setName("next_idx");
						paging_cookie.setValue(to_string(ret.idx)); // -1 = no more data to load
						paging_cookie.setPath("/friend/");
						res.set(res.SET_COOKIE, paging_cookie.toString());
						
						// if need to go to next page, render a button
						if (ret.idx != -1){
							string next_btt = "<form method=\"GET\" action=\"/friend/next\"><input type=\"submit\" value=\">>\"/></form><br>";
							friendListHTMLCode.append(next_btt);
						}
					}
					
					else {
						friendListHTMLCode = "<p> Something went wrong while loading your friends list. </p>";
					}
				}
			} else {
				friendListHTMLCode = "<p> Something went wrong while loading your friends list. </p>";
			}
			
		}
		
		
		// read HTML file as string 
		
		// put render code into template string
		Poco::format(htmlCode, ZRequestHandlerFactory::friendString, pendingHTMLCode, friendListHTMLCode);
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
		res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
		res.send().flush();
	}
}

/**
 * POST API: /friend/add
 * @param req
 * @param res
 * @param uid - userID getting from cookies
 */
void FriendRequestHandler::handleAddFriend(HTTPServerRequest& req, HTTPServerResponse& res, int uid){
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
			res.setStatusAndReason(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, reason);
			res.send().flush();
			return;
		}
		
		// Create a new friend request
		FriendRequest request;
		request.p_send_req = uid;
		request.p_recv_req = fr_id;
		request.greeting = greeting;
		
		// calling async api
		ErrorCode::type code = _conn->client()->addFriend(request);
		
		if(code == ErrorCode::SUCCESS){
			res.setStatus(HTTPResponse::HTTP_OK);
		} else if (code == ErrorCode::DUPLICATED_REQUEST){
			res.setStatus(HTTPResponse::HTTP_PRECONDITION_FAILED);
		}
		
		res.send().flush();
	} catch (...){ // some thing wrong
		Application::instance().logger().error("Error-Add Friend Request");
		return res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
	}
}

void FriendRequestHandler::handleAcceptRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& res, int uid){
	try {
		istream& body_stream = req.stream();
		HTMLForm form(req, body_stream);
		int requestId = stoi(form.get("friend.requestId"));
		string isAccept = form.get("button_accept", "decline");
		
		// check whether user accept or reject the friend request
		if (isAccept == "decline"){
			return handleRejectRequest(req, res, uid, requestId);
		}
		
		// get id from session management
		int userId = uid;
		
		// call api
		ErrorCode::type code = _conn->client()->acceptRequest(userId, requestId);
		
		// handle response
		if (code == ErrorCode::INVALID_PARAMETER){
			string reason = "Request Id is incorrect!";
			res.setStatusAndReason(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, reason);
		} else {
			res.setStatus(HTTPResponse::HTTP_OK);
			res.redirect("/friend");
		}
		
		res.send().flush();
	} catch (...){
		Application::instance().logger().error("Error-Accept Friend Request");
		return res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
	}
}

void FriendRequestHandler::handleRejectRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& res, int uid, int requestId){
	try {
		// get id from session management
		int userId = uid;
		
		// call api
		ErrorCode::type code = _conn->client()->declineRequest(userId, requestId);
		
		// handle response
		if (code == ErrorCode::INVALID_PARAMETER){
			string reason = "Request Id is incorrect!";
			res.setStatusAndReason(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, reason);
		} else {
			res.setStatus(HTTPResponse::HTTP_OK);
			res.redirect("/friend");
		}
		
		res.send().flush();
	} catch (...){
		Application::instance().logger().error("Error-Reject Friend Request");
		return res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
	}
}

void FriendRequestHandler::handleLoadmoreRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& res, int uid){
	try {
		istream& body_stream = req.stream();
		HTMLForm form(req, body_stream);
		string nextPos = form.get("next");
		
		// filter case call with next=-1 (already render all friends). Just return bcz it will never happen
		if (nextPos == "-1"){
			return;
		}
		
		// call API
		listFriendResult ret;
		_conn->client()->viewFriendList(ret, uid, stoi(nextPos), 20);
		
		// set header
		res.set("next", to_string(ret.idx));
		
		// render list
		// user data
		ProfileConnection *profileConn;
		GetUserResult userRet;
		while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));
		string data;
		for (int i=0; i < ret.friendList.size(); i++){
			int fr_id = ret.friendList[i];
			profileConn->client()->GetProfile(userRet, fr_id);
			bool online = userRet.profile.last_active_time == -1;
			string stt = online ? "online" : "offline";
			string friends = "<div id=\"friend_" + to_string(userRet.profile.id) + "\" class=\"friend friend_" + stt + "\"> <h2> " + userRet.profile.name + "</h2></div>";
			data.append(friends);
		}
		res.set("data", data);
		
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/html");
		res.send().flush();
	} catch (...){
		Application::instance().logger().information("Error - load more friends");
		return;
	}
}