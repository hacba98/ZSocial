/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "handlers.h"

void UIcontainer::loadUI(std::ostream& ostr) {
    ostr <<
            "<html>\n"
            "<head>\n"
            "<title>userProfileManager</title>\n"
            "</head>\n"
            "<body>\n"
            "<h1>USER PROFILE MANAGER</h1>\n"
            "<h2>GET Form</h2>\n"
            "<form method=\"GET\" action=\"/get\">\n"
            "ID <input type=\"text\" name=\"id\" size=\"31\" required> \n"
            "<input type=\"submit\" value=\"GET\">\n"
            "</form>\n"
            "<h2>Register Form</h2>\n"
            "<form method=\"POST\" action=\"/create\">\n"
            "NAME <input type=\"text\" name=\"name\" size=\"31\" required> <br>\n"
            "GENDER <input type=\"text\" name=\"gender\" size=\"31\" required> <br>\n"
            "DAY <input type=\"text\" name=\"day\" size=\"2\" required> \n"
            "MONTH <input type=\"text\" name=\"month\" size=\"2\" required> \n"
            "YEAR <input type=\"text\" name=\"year\" size=\"4\" required> <br>\n"
            "PHONE NUMBER <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "User Name <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "enter your Password <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "confirm your password <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "<input type=\"submit\" value=\"CREATE\">\n"
            "</form>\n"
            "<h2>UPDATE Form</h2>\n"
            "<form method=\"POST\" action=\"/update\">\n"
            "ID <input type=\"text\" name=\"id\" size=\"31\" required> <br>\n"
            "NAME <input type=\"text\" name=\"name\" size=\"31\" required> <br>\n"
            "GENDER <input type=\"text\" name=\"gender\" size=\"31\" required> <br>\n"
            "DAY <input type=\"text\" name=\"day\" size=\"2\" required> \n"
            "MONTH <input type=\"text\" name=\"month\" size=\"2\" required> \n"
            "YEAR <input type=\"text\" name=\"year\" size=\"4\" required> <br>\n"
            "PHONE NUMBER <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "User Name <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "enter your Password <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "confirm your password <input type=\"text\" name=\"day\" size=\"31\" required> <br>\n"
            "<input type=\"submit\" value=\"UPDATE\">\n"
            "</form>\n"
            "<h2>DELETE Form</h2>\n"
            "<form method=\"POST\" action=\"/delete\">\n"
            "ID     <input type=\"text\" name=\"id\" size=\"31\" required>\n"
            "<input type=\"submit\" value=\"DELETE\">\n"
            "</form>\n";
};

void GETRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("GET Request from " + request.clientAddress().toString());

    HTMLForm form(request, request.stream());

    int id = atoi(form.get("id", "-1").c_str());

    userProfileResult ret;
    if (id != -1) {
        client->Get(ret, id);
    }

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    std::ostream& ostr = response.send();

    UIcontainer::loadUI(ostr);
    ostr << "<h1>GET result</h1>\n";

    if (id != -1 && ret.errorCode == 0) {
        ostr << "<h2>Profile</h2><p>\n";

        int d, m, y;
        ret.profile.getDMY(d, m, y);

        ostr << "id:     " << ret.profile.id << "<br>\n";
        ostr << "name:   " << ret.profile.name << "<br>\n";
        ostr << "gender: " << ret.profile.gender << "<br>\n";
        ostr << "birth:  " << d << "/" << m << "/" << y << "<br>\n";
        ostr << "</p>";
    } else {
        ostr << "<h2>No Profile</h2>\n";
    }

    ostr << "</body>\n";
}

void CREATERequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("CREATE Request from " + request.clientAddress().toString());

    HTMLForm form(request, request.stream());

    string name = form.get("name", "No one will name like this");
    string gender = form.get("gender", "");
    int d = atoi(form.get("day", "-1").c_str());
    int m = atoi(form.get("month", "-1").c_str());
    int y = atoi(form.get("year", "-1").c_str());
    bool ok = (name != "No one will name like this") && (gender != "") && (d != -1) && (m != -1) && (y != -1);
    userProfile profile(name, gender, d, m, y);

    userProfileResult ret;
    if (ok) {
        client->Create(ret, profile);
    }

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    std::ostream& ostr = response.send();

    UIcontainer::loadUI(ostr);
    ostr << "<h1>CREATE result</h1>\n";

    if (ret.errorCode == 0 && ok) {
        ostr << "<p> SUCCESS </p> <br>\n"
                "<p> Your id is : " << ret.profile.id << " </p>\n";
    } else {
        ostr << "<p> NOT SUCCESS </p>\n";
    }

    ostr << "</body>\n";
}

void UPDATERequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("UPDATE Request from " + request.clientAddress().toString());

    HTMLForm form(request, request.stream());

    int id = atoi(form.get("id", "-1").c_str());
    string name = form.get("name", "No one will name like this");
    string gender = form.get("gender", "");
    int d = atoi(form.get("day", "-1").c_str());
    int m = atoi(form.get("month", "-1").c_str());
    int y = atoi(form.get("year", "-1").c_str());
    bool ok = (id != -1) && (name != "No one will name like this") && (gender != "") && (d != -1) && (m != -1) && (y != -1);
    userProfile profile(name, gender, d, m, y);

    int ret;
    if (ok) {
        ret = client->Update(id, profile);
    }

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    std::ostream& ostr = response.send();


    UIcontainer::loadUI(ostr);
    ostr << "<h1>UPDATE result</h1>\n";

    if (ret == 0 && ok) {
        ostr << "<p> SUCCESS </p> <br>\n";
    } else {
        ostr << "<p> NOT SUCCESS </p>\n";
    }

    ostr << "</body>\n";
}

void DELETERequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("DELETE Request from " + request.clientAddress().toString());

    HTMLForm form(request, request.stream());

    int id = atoi(form.get("id", "-1").c_str());

    int ret;
    if (id != -1) {
        ret = client->Delete(id);
    }

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    std::ostream& ostr = response.send();


    UIcontainer::loadUI(ostr);
    ostr << "<h1>DELETE result</h1>\n";

    if (ret == 0) {
        ostr << "<p> SUCCESS </p> <br>\n";
    } else {
        ostr << "<p> NOT SUCCESS </p>\n";
    }

    ostr << "</body>\n";
}

void FormRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request from " + request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    std::ostream& ostr = response.send();
    UIcontainer::loadUI(ostr);

    ostr << "</body>\n";
}