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
	if (url == "/dashboard"){
		res.setStatus(HTTPResponse::HTTP_OK);
		res.setContentType("text/html");
		
		try {
			url = "./src/dashboard.html";
			res.sendFile(url, "text/html");
		} catch (Exception e){
			cout << e.message() << endl;
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