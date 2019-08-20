/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ZRequestHandler.h"


void ProfileRequestHandler::handleRequest(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res){

    std::string url = req.getURI();
    if (url.find("/profile/login") == 0)
        return handleLogin(req, res);
    else if (url.find("/profile/register") == 0) {
        return handleRegister(req, res);
    }else if (url.find("/profile/update") == 0) {
        return handleUpdate(req, res);
    }else if (url.find("/profile/logout") == 0) {
        return handleLogout(req, res);
    }

}

void ProfileRequestHandler::handleRegister(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse& res){
    Application& app = Application::instance();
    app.logger().information("CREATE Request from " + req.clientAddress().toString());

    HTMLForm form(req, req.stream());

    string name = form.get("name", "No one will name like this");
    string gender = form.get("gender", "");

    long phoneNumber = (long) atoi(form.get("phonenumber", "-1").c_str());
    string username = form.get("username", "");
    string password = form.get("password", "");
    string repassword = form.get("repassword", "");
    bool ok = (name != "No one will name like this") && (gender != "") && (phoneNumber != -1l) &&
            (username != "") && (password != "") && (repassword == password);

    Poco::DateTime birthDate;
    int tz_diff;
    Poco::DateTimeParser::parse("%Y-%n-%e", form.get("birth", "1900-01-01"), birthDate, tz_diff);
    int birth = (int)(birthDate.timestamp().epochTime());
    UserProfile profile;
    TOOL::setProfile(profile, name, gender, birth, phoneNumber, username, password);

    CreateUserResult ret;
    if (ok) {
        _conn->client()->CreateProfile(ret, profile);
    }

    if (ret.errorCode == ErrorCode::SUCCESS && ok) { // valid case need to response token
        res.setStatus(HTTPResponse::HTTP_OK);
        
        loginResult loginRet;
        _conn->client()->LoginById(loginRet,ret.id);
        
        if (loginRet.code != ErrorCode::SUCCESS){
            res.set("valid", "login_fail");
            res.send().flush();
            return;
        }
        
        HTTPCookie cookie;
        cookie.setMaxAge(60*60*24);
        cookie.setName("zuid");
        cookie.setValue(ZRequestHandlerFactory::genCookie(loginRet.profile));
        cookie.setDomain("10.40.30.214");
        cookie.setPath("/");
        res.set(res.SET_COOKIE, cookie.toString());
    } else {
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        res.set("valid", "duplicate_username");
    }

    res.send().flush();
};

using apache::thrift::to_string;
void ProfileRequestHandler::handleLogin(
            Poco::Net::HTTPServerRequest &req,
            Poco::Net::HTTPServerResponse &res){
	// parsing data: username and password
	istream& body_stream = req.stream();
	HTMLForm form(req, body_stream);
	string username = form.get("username");
	string password = form.get("password");
	
	// using given connection to process server bussiness
	loginResult loginRet;
	_conn->client()->Login(loginRet, username, password);
	
	if (loginRet.code == ErrorCode::SUCCESS){ // valid case need to response token
		res.setStatus(HTTPResponse::HTTP_OK);
		
		// store token object in cookie
		HTTPCookie cookie;
		cookie.setMaxAge(60*60*24); // 1 year(day :v) life time
		cookie.setName("zuid");
		cookie.setValue(ZRequestHandlerFactory::genCookie(loginRet.profile));
		cookie.setDomain("10.40.30.214");
                
		cookie.setPath("/");
		res.set(res.SET_COOKIE, cookie.toString());
		
		res.set("valid", "true");
	} else if (loginRet.code == ErrorCode::USER_NOT_FOUND){
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	
	res.send().flush();
}

void ProfileRequestHandler::handleUpdate(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
	Application& app = Application::instance();
	app.logger().information("UPDATE Request from " + req.clientAddress().toString());

	NameValueCollection nvc;
	req.getCookies(nvc);
	string uid = nvc.get("zuid", "no_cookies");

	// no cookie case -> redirect login
	if (uid == "no_cookies") {
		res.setStatus(HTTPResponse::HTTP_NONAUTHORITATIVE);
		res.set("valid", "false");
		res.redirect("/login");
                res.send().flush();
		return;
	}

	// valid cookie: false -> redirect login
	SimpleProfile token_;
	if (!ZRequestHandlerFactory::validCookie(token_, uid)) {
		res.setStatus(HTTPResponse::HTTP_NONAUTHORITATIVE);
		res.set("valid", "false");
                res.redirect("/login");
		res.send().flush();
		return;
	}

	HTMLForm form(req, req.stream());

	// get id from token
	int id = token_.id;
	//int id = atoi(uid.c_str());
	string name = form.get("name", "No one will name like this");
	string gender = form.get("gender", "");

	long phoneNumber = (long) atoi(form.get("phonenumber", "0").c_str());
	
	string username = form.get("username", "");
	string password = form.get("password", "");
	string repassword = form.get("repassword", "");
	bool ok = (id != -1) && (repassword == password);

	Poco::DateTime birthDate;
	int tz_diff;
	Poco::DateTimeParser::parse("%Y-%n-%e", form.get("birth", "1900-01-01"), birthDate, tz_diff);
	int birth = (int) (birthDate.timestamp().epochTime());

	UserProfile profile;
	TOOL::setProfile(profile, name, gender, birth, phoneNumber, username, password);

	ErrorCode::type ret;
	if (ok) {
		ret = _conn->client()->UpdateProfile(profile, id);
	}

	if (ret == ErrorCode::SUCCESS && ok) {
		res.setStatus(HTTPResponse::HTTP_OK);
		res.set("valid", "true");
	} else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	res.send().flush();
};

void ProfileRequestHandler::handleLogout(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
	Application& app = Application::instance();
	app.logger().information("LOGOUT Request from " + req.clientAddress().toString());

	NameValueCollection nvc;
	req.getCookies(nvc);
	string uid = nvc.get("zuid", "no_cookies");

	// no cookie case -> redirect login
	if (uid == "no_cookies") {
		res.setStatus(HTTPResponse::HTTP_NONAUTHORITATIVE);
		res.set("valid", "false");
                res.redirect("/login");
		res.send().flush();
		return;
	}

	// valid cookie: false -> redirect login
	SimpleProfile token_;
	if (!ZRequestHandlerFactory::validCookie(token_, uid)) {
		res.setStatus(HTTPResponse::HTTP_NONAUTHORITATIVE);
		res.set("valid", "false");
                res.redirect("/login");
		res.send().flush();
		return;
	}

	// get id from session management
	int id = token_.id;

	ErrorCode::type ret;
	if (id != -1) {
		ret = _conn->client()->Logout(id);
	}

	HTTPCookie cookie;
	cookie.setMaxAge(0);
	cookie.setName("zuid");
	cookie.setValue("0");
	cookie.setDomain("10.40.30.214");
        //cookie.setDomain("localhost");
	cookie.setPath("/");
	res.set(res.SET_COOKIE, cookie.toString());

	if (ret == ErrorCode::SUCCESS) {
		res.setStatus(HTTPResponse::HTTP_OK);
		res.set("valid", "true");
	} else {
		res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
		res.set("valid", "false");
	}
	res.redirect("/login");
};
