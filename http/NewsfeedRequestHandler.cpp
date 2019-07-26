/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZRequestHandler.h"

//////////////////////////////////////
// main control flow

void NewsFeedRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
	std::string url = req.getURI();
	if (url.find("/newsFeed/create") == 0)
		return handleCreateRequest(req, res);
	else if (url.find("/newsFeed/update") == 0)
		return handleUpdateRequest(req, res);
	else if (url.find("/newsFeed/delete") == 0)
		return handleDeleteRequest(req, res);
	else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.send().flush();
	}
};

//////////////////////////////////////
// handler for create new feed request

void NewsFeedRequestHandler::handleCreateRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
	Poco::Util::Application::instance().logger().information("ADD FEED Request from " + req.clientAddress().toString());
	istream& body_stream = req.stream();
	HTMLForm form(req, body_stream);
	string content = form.get("content");

	NameValueCollection nvc;
	req.getCookies(nvc);
	string uid = nvc.get("zuid", "no_cookies");

	// case that dont have cookie -> guard - force to login again
	if (uid == "no_cookies") {
		return res.redirect("/login");
	}

	// valid cookie: false -> redirect login
	token token_;
	if (!ZRequestHandlerFactory::validCookie(token_, uid)) {
		return res.redirect("/login");
		;
	}

	int id = token_.zuid;

	FeedCreateResult ret;
	_conn->client()->createNewsFeed(ret, id, content, 0); //status is 0 because no IDEA what it do
	if (ret.exitCode == 0) {
		res.setStatus(HTTPResponse::HTTP_OK);
	} else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
	}
	res.redirect("/feed");
};

//////////////////////////////////////
// handler for update newsfeed request

void NewsFeedRequestHandler::handleUpdateRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
	Poco::Util::Application::instance().logger().information("UPDATE FEED Request from " + req.clientAddress().toString());
	istream& body_stream = req.stream();
	HTMLForm form(req, body_stream);
	string content = form.get("content");
	int f_id = atoi(form.get("id").c_str());

	FeedUpdateResult ret;
	_conn->client()->updateNewsFeed(ret, f_id, content, 0); //status is 0 because no IDEA what it do
	if (ret.exitCode == 0) {
		res.setStatus(HTTPResponse::HTTP_OK);
		res.set("valid", "true");
	} else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	res.send().flush();
};

//////////////////////////////////////
// handler for delete newsfeed request

void NewsFeedRequestHandler::handleDeleteRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
	Poco::Util::Application::instance().logger().information("Delete FEED Request from " + req.clientAddress().toString());
	istream& body_stream = req.stream();
	HTMLForm form(req, body_stream);
	int f_id = atoi(form.get("id").c_str());

	NameValueCollection nvc;
	req.getCookies(nvc);
	string uid = nvc.get("zuid", "no_cookies");

	// case that dont have cookie -> guard - force to login again
	if (uid == "no_cookies") {
		return res.redirect("/login");
	}

	// valid cookie: false -> redirect login
	token token_;
	if (!ZRequestHandlerFactory::validCookie(token_, uid)) {
		return res.redirect("/login");
	}

	int id = token_.zuid;

	FeedDeleteResult ret;
	_conn->client()->deleteNewsFeed(ret, f_id, id); //status is 0 because no IDEA what it do
	if (ret.exitCode == 0) {
		res.setStatus(HTTPResponse::HTTP_OK);
		res.set("valid", "true");
	} else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	res.send().flush();
};
