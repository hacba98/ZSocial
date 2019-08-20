/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ZRequestHandler.h"
//static list<WebSocketRequestHandler> connectedSocket;
//////////////////////////////////////

void MessageRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) {
    std::string url = req.getURI();
    NameValueCollection nvc;
    req.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");
    SimpleProfile token_;
    bool valid = ZRequestHandlerFactory::validCookie(token_, uid);

    if (!valid) {// redirect to /login
        res.redirect("/login");
        return;
    }

    int id = token_.id;

    if (url.find("/message/sendmsg") == 0)
        return handleSendMsg(req, res, id);
    else if (url.find("/message/longpolling") == 0)
        return handleLongPolling(req, res, id);
    else {
        return handleLoadPage(req, res, id);
    }
};

void MessageRequestHandler::handleLoadPage(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, int uid) {
    res.setChunkedTransferEncoding(true);
    res.setContentType("text/html");
    
    HTMLForm form(req, req.stream());

    string _key = form.get("key", "-1");
    
    std::ostream& ostr = res.send();
    string result, frid = "", msg = "";
    FriendConnection * friendConn;
    listFriendResult friRet;
    while (!(friendConn = ZRequestHandlerFactory::friendPool()->borrowObject(100)));
    friendConn->client()->viewFriendList(friRet, uid, 0, 30);

    ProfileConnection *profileConn;
    ListProfileResult listF;
    while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));

    MessageConnection *msgConn;
    while (!(msgConn = ZRequestHandlerFactory::messagePool()->borrowObject(100)));
    KeyMsgResult key;
    ListMessageResult listMsg;
    profileConn->client()->getList(listF, friRet.friendList);
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    for (int i = 0; i < friRet.size; i++) {
        msgConn->client()->getKey(key, uid, listF.profiles[i].id);
        msg += "<div id=" + key.key + " class=\"tabcontent\"><h3>" + listF.profiles[i].name + "</h3>";

        IB_Info boxinfo;
        msgConn->client()->getBoxinfo(boxinfo, key.key);
        root->set(boxinfo._id, boxinfo.count);

        msgConn->client()->getMessages(listMsg, key.key, boxinfo.count);
        for (int j = 0; j < listMsg.result.count; j++) {
            if (listMsg.result.msglist[j].id_from == uid)
                msg += "<p align=\"right\">" + listMsg.result.msglist[j].content + "</p>";
            else msg += "<p>" + listMsg.result.msglist[j].content + "</p>";
        }
        msg += "</div>";
        //        if (i == 0)
        frid += "<li onclick = \"openCity(event, '" + key.key + "')\" id=\"k" + key.key + "\" ><a>" + listF.profiles[i].name + +"</a></li>";
        //        else frid += "<li onclick = \"openCity(event, '" + key.key + "')\" id=\"k" + key.key + "\" ><a>" + listF.profiles[i].name + "</a></li>";


    }
    string mmm;
    stringstream ss;
    string setOpenCity ="";
    if (_key != "-1") {
        if(stoi(_key) > uid){
            _key= to_string(uid) + "_" + _key;
        }else{
            _key= _key + "_" + to_string(uid);
        }
        setOpenCity ="$(k"+_key+").click()";
    };
    root->stringify(ss);
    mmm = ss.str();
    Poco::format(result, ZRequestHandlerFactory::messageString, frid, msg, mmm, uid,setOpenCity);
    ostr << result;
    ostr.flush();
    ZRequestHandlerFactory::messagePool()->returnObject(msgConn);
    ZRequestHandlerFactory::friendPool()->returnObject(friendConn);
    ZRequestHandlerFactory::profilePool()->returnObject(profileConn);
}

bool MessageRequestHandler::checkin(string key, int id) {
    string inbox_id = key;
    size_t prev = 0, pos = 0;
    do {
        pos = inbox_id.find("_", prev);
        if (pos == string::npos) pos = inbox_id.length();
        string token = inbox_id.substr(prev, pos - prev);
        if (!token.empty()) {
            int32_t user = stoi(token);
            if (user == id) return true;
        }
        prev = pos + 1;
    } while (pos < inbox_id.length() && prev < inbox_id.length());
    return false;
}

void MessageRequestHandler::handleSendMsg(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, int uid) {
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    string inbox_id = form.get("inbox_id");
    string content = form.get("content");
    MessageCreateResult newMsg;
    MessageConnection *msgConn;
    while (!(msgConn = ZRequestHandlerFactory::messagePool()->borrowObject(100)));
    msgConn->client()->createMessage(newMsg, uid, inbox_id, content);
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("inbox_id", inbox_id);
    root->set("content", content);
    root->set("id_from", uid);
    root->set("new_count", newMsg.new_count);
    for (vector<MessageRequestHandler *>::iterator it = longPolling.begin(); it != longPolling.end(); ++it) {
        if ((checkin(inbox_id, (*it)->uid))&&((*it)->flag == 0)) {
            (*it)->data = root;
            //            if (((*it)->uid != uid)) {
            //                (*it)->flag = 1;
            //            } else {
            //                (*it)->flag = 2;
            //            }
            (*it)->flag = 1;
        }
    }
    //    int c=1; // use websocket
    //    for (vector<WebSocketRequestHandler *>::iterator it = connectedSocket.begin(); it != connectedSocket.end(); ++it) {
    //        if (((*it)->uid != uid)&&(checkin(inbox_id, (*it)->uid))) {
    //            cout<<c++<<" ";
    //            (*it)->ws->sendFrame(mmm.c_str(), mmm.size(), WebSocket::FRAME_TEXT);
    //
    //        }
    //    }
    //    cout<<"end\n";

    res.setStatus(HTTPResponse::HTTP_OK);
    res.send().flush();
    ZRequestHandlerFactory::messagePool()->returnObject(msgConn);
}

void MessageRequestHandler::handleLongPolling(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res, int uid) {
    istream& body_stream = req.stream();
    HTMLForm form(req, body_stream);
    string json = form.get("boxinfo");

    Parser parser;
    Var result = parser.parse(json);
    Object::Ptr object = result.extract<Object::Ptr>();

    this->uid = uid;
    this->flag = 0;
    longPolling.push_back(this);

    FriendConnection * friendConn;
    listFriendResult friRet;
    while (!(friendConn = ZRequestHandlerFactory::friendPool()->borrowObject(100)));
    friendConn->client()->viewFriendList(friRet, uid, 0, 30);

    ProfileConnection *profileConn;
    ListProfileResult listF;
    while (!(profileConn = ZRequestHandlerFactory::profilePool()->borrowObject(100)));

    MessageConnection *msgConn;
    while (!(msgConn = ZRequestHandlerFactory::messagePool()->borrowObject(100)));
    KeyMsgResult key;
    ListMessageResult listMsg;
    profileConn->client()->getList(listF, friRet.friendList);

    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    Poco::JSON::Object::Ptr info = new Poco::JSON::Object();
    Poco::JSON::Array::Ptr msgArray = new Poco::JSON::Array();
    int ncount = 0; // count new message

    for (int i = 0; i < friRet.size; i++) {
        msgConn->client()->getKey(key, uid, listF.profiles[i].id);
        IB_Info boxinfo;
        msgConn->client()->getBoxinfo(boxinfo, key.key);
        info->set(boxinfo._id, boxinfo.count);
        if (object->has(key.key)) {
            Var box = object->get(key.key);
            int value = box.convert<int>();
            if (value < boxinfo.count) {
                msgConn->client()->getNewMessages(listMsg, key.key, value, boxinfo.count);
                for (int j = 0; j < listMsg.result.count; j++) {
                    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
                    obj->set("inbox_id", key.key);
                    obj->set("id_from", listMsg.result.msglist[i].id_from);
                    obj->set("content", listMsg.result.msglist[i].content);
                    msgArray->set(ncount++, obj);
                }
            }
        }
    }


    if (ncount > 0) {
        root->set("info", info);
        root->set("msg", msgArray);

    } else {
        while (this->flag == 0) {
        }
        Var idb = this->data->get("inbox_id");
        string value = idb.convert<string>();

        Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
        obj->set("inbox_id", value);
        obj->set("id_from", this->data->get("id_from"));
        obj->set("content", this->data->get("content"));
        msgArray->set(ncount++, obj);
        root->set("msg", msgArray);
        info->set(value, this->data->get("new_count"));


        root->set("info", info);
    }
    string mmm;
    stringstream ss;
    root->stringify(ss);
    mmm = ss.str();
    res.set("data", mmm);
    res.setStatus(HTTPResponse::HTTP_OK);
    res.send().flush();
    for (vector<MessageRequestHandler *>::iterator it = longPolling.begin(); it != longPolling.end(); ++it) {
        if (*it == this) {
            longPolling.erase(it);
            break;
        }
    }
    ZRequestHandlerFactory::messagePool()->returnObject(msgConn);
    ZRequestHandlerFactory::friendPool()->returnObject(friendConn);
    ZRequestHandlerFactory::profilePool()->returnObject(profileConn);
}

void WebSocketRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    NameValueCollection nvc;
    request.getCookies(nvc);
    string uid = nvc.get("zuid", "no_cookies");
    SimpleProfile token_;
    bool valid = ZRequestHandlerFactory::validCookie(token_, uid);

    if (!valid) {// redirect to /login
        response.redirect("/login");
        return;
    }
    this->uid = token_.id;

    try {
        ws = new WebSocket(request, response);
        //app.logger().information("WebSocket connection established.");
        //Poco::Timespan timeOut(50, 0);
        ws->setKeepAlive(true);
        request.set("Connection", "Upgrade");
        connectedSocket.push_back(this);
        char buffer[1024];
        int flags = 0;
        int n;

        do {

            n = ws->receiveFrame(buffer, sizeof (buffer), flags);
            if (n > 0) {
                if ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_BINARY) {
                    // process and send out to all other clients

                    ws->sendFrame(buffer, n, flags);
                }
            }

        } while (n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
        //app.logger().information("WebSocket connection closed.");
        for (vector<WebSocketRequestHandler *>::iterator it = connectedSocket.begin(); it != connectedSocket.end(); ++it) {
            if (*it == this) {
                connectedSocket.erase(it);
                break;
            }
        }
        cout << "Timeout" << endl;
    } catch (WebSocketException& exc) {
        //app.logger().log(exc);
        cout << "die: " << endl;
        switch (exc.code()) {
            case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
                // fallthrough
            case WebSocket::WS_ERR_NO_HANDSHAKE:
            case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
            case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
                response.setContentLength(0);
                response.send();
                break;
        }
    }
}