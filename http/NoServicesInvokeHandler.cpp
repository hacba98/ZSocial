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
	res.set("Cache-Control", "no-cache");
	// check for cookie
	NameValueCollection nvc;
	req.getCookies(nvc);
	string uid = nvc.get("zuid", "no_cookies");

	// serve icon request from browser
	if (url == "/favicon.ico") {
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

	
	
	// valid cookie 
	token token_;
	bool valid = ZRequestHandlerFactory::validCookie(token_, uid);
	
	

	// serve login - entry page
	if (url.empty() || url == "/" || url == "/login") {
		bool flag = true;
		// if cookie has data
		if (!(uid == "no_cookies") && valid) {
			res.redirect("/dashboard");
			return;
		}

		if (flag) {
			res.setStatus(HTTPResponse::HTTP_OK);
			res.setContentType("text/html");
			//res.set("Cache-Control", "no cache");

			try {
				url = "./src/index.html";
				res.sendFile(url, "text/html");
			} catch (Exception e) {
				cout << e.message() << endl;
			}
			return;
		}
	}
	
	// Guard
	if (uid == "no_cookies")
		res.redirect("/login");
	
	if (!valid)
		res.redirect("/login");
	
	if (url.find("/dashboard") == 0) {
		dashBoard(req, res, token_.zuid);
		return;
	}
	if (url.find("/feed") == 0) {
		myFeed(req, res, token_.zuid);
		return;
	}
	if (url.find("/myprofile") == 0) {
		myProfile(req, res, token_.zuid);
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
		return;
	}

	// serve unknow path
	res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
	res.redirect("/");
	return;
}

void NoServicesInvokeHandler::dashBoard(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, int uid) {
	res.setChunkedTransferEncoding(true);
	res.setContentType("text/html");

	try {

		string result;
		string feedString, friendString;
		int user_id = uid;

		FriendConnection * friendConn;
		listFriendResult friRet;
		while (!(friendConn = ZRequestHandlerFactory::friendPool()->borrowObject(100)));

		friendConn->client()->viewFriendList(friRet, user_id, 0, 20); // get first 20 friends

		FeedCountResult feedRet;
		ListFeedResult listFeed;
		NewsFeedConnection *feedConn;
		while (!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds


		// user data
		ProfileConnection *profileConn;
		GetUserResult userRet;
		while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));



		for (auto fr = friRet.friendList.begin(); fr != friRet.friendList.end(); ++fr) {
			feedConn->client()->getFeedCount(feedRet, *fr);
			feedConn->client()->getListFeed(listFeed, *fr, feedRet.result, 1);


			profileConn->client()->GetProfile(userRet, *fr);
			bool online = userRet.profile.last_active_time == -1;
			string stt = online ? "online" : "offline";
			string friends = "<div id=\"friend_" + to_string(userRet.profile.id) + "\" class=\"friend friend_" + stt + "\"> <h2> " + userRet.profile.name + "</h2></div>";
			friendString.append(friends);

			if (listFeed.exitCode == 0) {

				for (auto i = listFeed.result.feedlist.begin(); i != listFeed.result.feedlist.end(); ++i) {
					int d, m, y, hh, pp;
					TOOL::getDMY(i->edit_time, d, m, y, hh, pp);
					string date = Poco::NumberFormatter::format(y) + "-" + Poco::NumberFormatter::format0(m, 2) + "-" + Poco::NumberFormatter::format0(d, 2)
						+ "  " + Poco::NumberFormatter::format0(hh, 2) + ":" + Poco::NumberFormatter::format0(pp, 2);
					string feed = "<div class=\"card\"> <h1>" + userRet.profile.name + "</h1>" +
						"<p class=\"price\">" + date + "</p>" +
						"<p>" + i->content + "</p><p><button>Like</button></p></div><br><br>";
					feedString.append(feed);
				}
			}
		}
		ZRequestHandlerFactory::newsfeedPool()->returnObject(feedConn);
		ZRequestHandlerFactory::profilePool()->returnObject(profileConn);
		ZRequestHandlerFactory::friendPool()->returnObject(friendConn);
		Poco::format(result, ZRequestHandlerFactory::dashboardString, listFeed.result.nex.id, listFeed.result.nex.post, feedString, friendString);
		std::ostream& ostr = res.send();
		ostr << result;
	} catch (Exception e) {
		Poco::Util::Application::instance().logger().error(e.message());
	}
};

void NoServicesInvokeHandler::myProfile(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, int uid) {
	res.setChunkedTransferEncoding(true);
	res.setContentType("text/html");

	try {
		GetUserResult ret;
		ProfileConnection *profileConn;
		while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100))); // timeout 100 miliseconds
		profileConn->client()->GetProfile(ret, uid);

		ZRequestHandlerFactory::profilePool()->returnObject(profileConn);

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
};

void NoServicesInvokeHandler::myFeed(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, int uid) {
	res.setChunkedTransferEncoding(true);
	res.setContentType("text/html");

	try {
		// retrieve list friend's newsfeed
		int user_id = uid;
		FeedCountResult feedRet;
		ListFeedResult listFeed;
		NewsFeedConnection *feedConn;
		while (!(feedConn = ZRequestHandlerFactory::newsfeedPool()->borrowObject(100))); // timeout 100 miliseconds
		feedConn->client()->getFeedCount(feedRet, user_id);
		feedConn->client()->getListFeed(listFeed, user_id, feedRet.result, 3);

		string result, feedString;
		if (listFeed.exitCode == 0) {
			for (auto i = listFeed.result.feedlist.begin(); i != listFeed.result.feedlist.end(); ++i) {
				int d, m, y, hh, pp;
				TOOL::getDMY(i->edit_time, d, m, y, hh, pp);
				string date = Poco::NumberFormatter::format(y) + "-" + Poco::NumberFormatter::format0(m, 2) + "-" + Poco::NumberFormatter::format0(d, 2)
					+ "  " + Poco::NumberFormatter::format0(hh, 2) + ":" + Poco::NumberFormatter::format0(pp, 2);
				string feed = "<form class=\"card\" name=\"feed_no_" + std::to_string(i->id) + "\" onsubmit=\"updateFeed('" + std::to_string(i->id) + "')\">\n" +
					"<p class=\"price\"> Date:" + date + "</p>\n" +
					"<textarea class=\"status_post\" rows=\"4\" name=\"content\">" + i->content + "</textarea><br>\n" +
					"<input type =\"button\" class=\"updateBTN\" onClick=\"updateFeed('" + std::to_string(i->id) + "')\" value=\"UPDATE\">\n" +
					"<input type =\"button\" class=\"deleteBTN\" onClick=\"removeFeed('" + std::to_string(i->id) + "')\" value=\"REMOVE\">\n" +
					"</form> <hr><br><br>\n";
				feedString.append(feed);
			}
		}
		ZRequestHandlerFactory::newsfeedPool()->returnObject(feedConn);

		Poco::format(result, ZRequestHandlerFactory::myfeedString, listFeed.result.nex.id, listFeed.result.nex.post, feedString);
		std::ostream& ostr = res.send();
		ostr << result;
	} catch (Exception e) {
		Poco::Util::Application::instance().logger().error(e.message());
	}
};

////////////////////////////////////////////

void WebSocketHandler::handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& res) {
	try {
		// check for cookie
		NameValueCollection nvc;
		req.getCookies(nvc);
		string uid = nvc.get("zuid", "no_cookies");
		if (uid == "no_cookies")
			res.redirect("/login");

		// read token in request
		token token_;
		bool valid = ZRequestHandlerFactory::validCookie(token_, uid);
		if (!valid)
			res.redirect("/login");

		// data from cookie
		int zuid = token_.zuid;

		// establish new connection between client and server
		WebSocket ws(req, res);
		Application::instance().logger().information("Web-socket established successfully.");
		(*ZRequestHandlerFactory::clients())[zuid] = &ws; //register connected client 
		ZRequestHandlerFactory::onClientConnect(zuid);
		
		// TODO - Bug: client turn off connection will set them to be logout in DB
		// next time they visit home page they will be bypass login page with cookie
		// so their status need to set to online again

		char buffer[1024];
		int flags, n;

		// we need to sepecify timeout - 5s
		ws.setReceiveTimeout(Poco::Timespan(10, 0, 0, 5, 0));
		do {
			// ws will be blocked and waiting for new data to send back to client
			// in most case, receiveFrame will throw an exception
			// then we will gather new data from some way storage for this socket, mapping maybe
			// in case it return 0 value, that mean peer had been closed or shutdown
//			try {
//				n = ws.receiveFrame(buffer, sizeof (buffer), flags);
//			} catch (Poco::TimeoutException e) {	
//				// temp sol: take a ping-pong process
////				if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_PING) {
////					ws.sendFrame(buffer, n, WebSocket::FRAME_OP_PONG);
////				} else {
////					// ping the client
////					string ping="ping";
////					ws.sendFrame(ping.c_str(), n, WebSocket::FRAME_OP_PING);
////				}
//				
//				// timeout - send a ping message
//				string ping="ping";
//				ws.sendFrame(ping.c_str(), ping.size(), WebSocket::FRAME_TEXT);
//				n = ws.receiveFrame(buffer, sizeof (buffer), flags);
//				cout << n << endl;
//				// if client response with pong message
//				// continue waiting
//			}
			
			n = ws.receiveFrame(buffer, sizeof (buffer), flags);
		} while (n > 0);
		//} while (n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
		
		Application::instance().logger().information("Timeout");
		// handle case where peer had been disconnected
		if (n == 0) {
			// storing user logout status in DB
			ProfileConnection *borrowObj;
			while (!(borrowObj = ZRequestHandlerFactory::profilePool()->borrowObject(100)));
			borrowObj->client()->Logout(zuid);
			ZRequestHandlerFactory::profilePool()->returnObject(borrowObj);
			
			// notificate all friends user logout status
			ZRequestHandlerFactory::onClientDisconnect(zuid);

			// remove from list of current clients
			ZRequestHandlerFactory::clients()->erase(zuid);

			Application::instance().logger().information(Logger::format("User $0 disconnected.", to_string(zuid)));
		}

	} catch (Poco::Exception& exc){
		Application::instance().logger().information(exc.message());
		switch (exc.code()) {
			case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
				res.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
				// fallthrough
			case WebSocket::WS_ERR_NO_HANDSHAKE:
			case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
			case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
				res.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
				res.setContentLength(0);
				res.send();
				break;
		}
	}
}
