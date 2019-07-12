/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "userProfileDB.h"

UserProfileDB::UserProfileDB(){
    
};
UserProfileDB::~UserProfileDB(){
    if (!db.close() || !keyStore.close() || !username_id.close()) {
        log->error("close database error");
    }
};

void UserProfileDB::initialize ( Poco::Util::Application &self ){
    std::string DB = self.config().getString("DBFile" ,"userProfiles.kch");
    std::string keyDB = self.config().getString("DBKeyStore","keyGen.kch");
    std::string usernameDB = self.config().getString("DBUserNameStore","user_id.kch");
    
    log = &self.logger();
    
    
    if (!db.open(DB, kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE) || 
            !keyStore.open(keyDB, kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE) ||
            !username_id.open(usernameDB, kyotocabinet::HashDB::OWRITER | kyotocabinet::HashDB::OCREATE)) {
        log->error("open database error");
    }
};
void UserProfileDB::reinitialize ( Poco::Util::Application &self ){
    Poco::Util::Subsystem::reinitialize(self);
};
void UserProfileDB::uninitialize(){
    log->information("uninitialize Database");
};

const char* UserProfileDB::name() const{
    return "kyoto cabinet database";
};


int UserProfileDB::getNextID(){
    std::string s;
    if (!keyStore.get("0", &s)) {
        if (!keyStore.set("0", "1")) {
            log->error(keyStore.error().name());
            return -1;
        }
        return 1;
    } else {
        return atoi(s.c_str());
    };
}

int UserProfileDB::setNextID(int newID){
    if (!keyStore.set("0", std::to_string(newID))) {
        log->error("error update key : %s" , db.error().name());
    }
}

bool UserProfileDB::isContainDB(int id) {
    if (id == 0) return false;
    std::string s;
    if (!db.get(std::to_string(id), &s)) {
        return false;
    };
    return true;
}

ErrorCode::type UserProfileDB::setDB(int id, UserProfile profile) {
    if (id == 0) return ErrorCode::OUT_OF_RANGE;
    std::string vaule = serializeProfile(profile);
    if (!db.set(std::to_string(id), vaule)) {
        return ErrorCode::OUT_OF_RANGE;
    }
    return ErrorCode::SUCCESS;
}

ErrorCode::type UserProfileDB::getDB(int id, UserProfile &profile) {
    if (id == 0) return ErrorCode::OUT_OF_RANGE;
    std::string vaule;
    if (!db.get(std::to_string(id), &vaule)) {
        return ErrorCode::USER_NOT_FOUND;
    };
    deserilizeProfile(vaule, profile);
    profile.id = id;
    return ErrorCode::SUCCESS;
}

ErrorCode::type UserProfileDB::removeDB(int id) {
    if (id == 0) return ErrorCode::OUT_OF_RANGE;
    db.remove(std::to_string(id));
    return ErrorCode::SUCCESS;
}

ErrorCode::type UserProfileDB::setUsername(std::string username , int id){
    if (!username_id.set(username, std::to_string(id))) {
        return ErrorCode::OUT_OF_RANGE;
    }
    return ErrorCode::SUCCESS;
};
ErrorCode::type UserProfileDB::rmUsername(std::string username){
    username_id.remove(username);
    return ErrorCode::SUCCESS;
};

ErrorCode::type UserProfileDB::getIdByName(int& _ret,std::string username){
    std::string vaule;
    if (!username_id.get(username, &vaule)) {
        return ErrorCode::USER_NOT_FOUND;
    };
    _ret = atoi(vaule.c_str());
    return ErrorCode::SUCCESS;
};

//https://stackoverflow.com/questions/19672155/apache-thrift-serialization-in-c
std::string UserProfileDB::serializeProfile(UserProfile obj) {
    boost::shared_ptr<TMemoryBuffer> transportOut(new TMemoryBuffer());
    boost::shared_ptr<TBinaryProtocol> protocolOut(new TBinaryProtocol(transportOut));
    obj.write(protocolOut.get());
    std::string serialized_string = transportOut->getBufferAsString();
    return serialized_string;
}

void UserProfileDB::deserilizeProfile(std::string serializeString, UserProfile &profile) {
    boost::shared_ptr<TMemoryBuffer> transportIn(new TMemoryBuffer());
    boost::shared_ptr<TBinaryProtocol> protocolIn(new TBinaryProtocol(transportIn));
    transportIn.get()->write((uint8_t*) serializeString.c_str(), serializeString.length());
    profile.read(protocolIn.get());
}