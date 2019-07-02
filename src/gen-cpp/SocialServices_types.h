/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef SocialServices_TYPES_H
#define SocialServices_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




struct ErrorCode {
  enum type {
    SUCCESS = 0,
    USER_NOT_FOUND = 1,
    INVALID_PARAMETER = 2,
    INVALID_SERVICE = 3,
    INTERNAL_ERROR = 4,
    DUPLICATED_REQUEST = 5,
    OUT_OF_RANGE = 6
  };
};

extern const std::map<int, const char*> _ErrorCode_VALUES_TO_NAMES;

class UserProfile;

class SimpleProfile;

class CreateUserResult;

class GetUserResult;

class ListProfileResult;

class FriendList;

class PendingRequest;

class FriendRequest;

class FriendData;

class pingResult;

class listFriendResult;

typedef struct _UserProfile__isset {
  _UserProfile__isset() : id(false), name(false), birth(false), gender(false), username(false), password(false), phoneNumber(false) {}
  bool id :1;
  bool name :1;
  bool birth :1;
  bool gender :1;
  bool username :1;
  bool password :1;
  bool phoneNumber :1;
} _UserProfile__isset;

class UserProfile {
 public:

  UserProfile(const UserProfile&);
  UserProfile& operator=(const UserProfile&);
  UserProfile() : id(0), name(), birth(0), gender(0), username(), password(), phoneNumber(0) {
  }

  virtual ~UserProfile() throw();
  int32_t id;
  std::string name;
  int32_t birth;
  bool gender;
  std::string username;
  std::string password;
  int64_t phoneNumber;

  _UserProfile__isset __isset;

  void __set_id(const int32_t val);

  void __set_name(const std::string& val);

  void __set_birth(const int32_t val);

  void __set_gender(const bool val);

  void __set_username(const std::string& val);

  void __set_password(const std::string& val);

  void __set_phoneNumber(const int64_t val);

  bool operator == (const UserProfile & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(name == rhs.name))
      return false;
    if (!(birth == rhs.birth))
      return false;
    if (!(gender == rhs.gender))
      return false;
    if (!(username == rhs.username))
      return false;
    if (!(password == rhs.password))
      return false;
    if (!(phoneNumber == rhs.phoneNumber))
      return false;
    return true;
  }
  bool operator != (const UserProfile &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UserProfile & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UserProfile &a, UserProfile &b);

inline std::ostream& operator<<(std::ostream& out, const UserProfile& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _SimpleProfile__isset {
  _SimpleProfile__isset() : id(false), name(false), gender(false) {}
  bool id :1;
  bool name :1;
  bool gender :1;
} _SimpleProfile__isset;

class SimpleProfile {
 public:

  SimpleProfile(const SimpleProfile&);
  SimpleProfile& operator=(const SimpleProfile&);
  SimpleProfile() : id(0), name(), gender(0) {
  }

  virtual ~SimpleProfile() throw();
  int32_t id;
  std::string name;
  bool gender;

  _SimpleProfile__isset __isset;

  void __set_id(const int32_t val);

  void __set_name(const std::string& val);

  void __set_gender(const bool val);

  bool operator == (const SimpleProfile & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(name == rhs.name))
      return false;
    if (!(gender == rhs.gender))
      return false;
    return true;
  }
  bool operator != (const SimpleProfile &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const SimpleProfile & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(SimpleProfile &a, SimpleProfile &b);

inline std::ostream& operator<<(std::ostream& out, const SimpleProfile& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _CreateUserResult__isset {
  _CreateUserResult__isset() : errorCode(false), id(false) {}
  bool errorCode :1;
  bool id :1;
} _CreateUserResult__isset;

class CreateUserResult {
 public:

  CreateUserResult(const CreateUserResult&);
  CreateUserResult& operator=(const CreateUserResult&);
  CreateUserResult() : errorCode((ErrorCode::type)0), id(0) {
  }

  virtual ~CreateUserResult() throw();
  ErrorCode::type errorCode;
  int32_t id;

  _CreateUserResult__isset __isset;

  void __set_errorCode(const ErrorCode::type val);

  void __set_id(const int32_t val);

  bool operator == (const CreateUserResult & rhs) const
  {
    if (!(errorCode == rhs.errorCode))
      return false;
    if (!(id == rhs.id))
      return false;
    return true;
  }
  bool operator != (const CreateUserResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const CreateUserResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(CreateUserResult &a, CreateUserResult &b);

inline std::ostream& operator<<(std::ostream& out, const CreateUserResult& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _GetUserResult__isset {
  _GetUserResult__isset() : errorCode(false), profile(false) {}
  bool errorCode :1;
  bool profile :1;
} _GetUserResult__isset;

class GetUserResult {
 public:

  GetUserResult(const GetUserResult&);
  GetUserResult& operator=(const GetUserResult&);
  GetUserResult() : errorCode((ErrorCode::type)0) {
  }

  virtual ~GetUserResult() throw();
  ErrorCode::type errorCode;
  UserProfile profile;

  _GetUserResult__isset __isset;

  void __set_errorCode(const ErrorCode::type val);

  void __set_profile(const UserProfile& val);

  bool operator == (const GetUserResult & rhs) const
  {
    if (!(errorCode == rhs.errorCode))
      return false;
    if (!(profile == rhs.profile))
      return false;
    return true;
  }
  bool operator != (const GetUserResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const GetUserResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(GetUserResult &a, GetUserResult &b);

inline std::ostream& operator<<(std::ostream& out, const GetUserResult& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ListProfileResult__isset {
  _ListProfileResult__isset() : errorCode(false), profiles(false) {}
  bool errorCode :1;
  bool profiles :1;
} _ListProfileResult__isset;

class ListProfileResult {
 public:

  ListProfileResult(const ListProfileResult&);
  ListProfileResult& operator=(const ListProfileResult&);
  ListProfileResult() : errorCode((ErrorCode::type)0) {
  }

  virtual ~ListProfileResult() throw();
  ErrorCode::type errorCode;
  std::vector<SimpleProfile>  profiles;

  _ListProfileResult__isset __isset;

  void __set_errorCode(const ErrorCode::type val);

  void __set_profiles(const std::vector<SimpleProfile> & val);

  bool operator == (const ListProfileResult & rhs) const
  {
    if (!(errorCode == rhs.errorCode))
      return false;
    if (!(profiles == rhs.profiles))
      return false;
    return true;
  }
  bool operator != (const ListProfileResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ListProfileResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ListProfileResult &a, ListProfileResult &b);

inline std::ostream& operator<<(std::ostream& out, const ListProfileResult& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _FriendList__isset {
  _FriendList__isset() : size(false), data(false) {}
  bool size :1;
  bool data :1;
} _FriendList__isset;

class FriendList {
 public:

  FriendList(const FriendList&);
  FriendList& operator=(const FriendList&);
  FriendList() : size(0) {
  }

  virtual ~FriendList() throw();
  int32_t size;
  std::set<int32_t>  data;

  _FriendList__isset __isset;

  void __set_size(const int32_t val);

  void __set_data(const std::set<int32_t> & val);

  bool operator == (const FriendList & rhs) const
  {
    if (!(size == rhs.size))
      return false;
    if (!(data == rhs.data))
      return false;
    return true;
  }
  bool operator != (const FriendList &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const FriendList & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(FriendList &a, FriendList &b);

inline std::ostream& operator<<(std::ostream& out, const FriendList& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _PendingRequest__isset {
  _PendingRequest__isset() : size(false), data(false) {}
  bool size :1;
  bool data :1;
} _PendingRequest__isset;

class PendingRequest {
 public:

  PendingRequest(const PendingRequest&);
  PendingRequest& operator=(const PendingRequest&);
  PendingRequest() : size(0) {
  }

  virtual ~PendingRequest() throw();
  int32_t size;
  std::set<int32_t>  data;

  _PendingRequest__isset __isset;

  void __set_size(const int32_t val);

  void __set_data(const std::set<int32_t> & val);

  bool operator == (const PendingRequest & rhs) const
  {
    if (!(size == rhs.size))
      return false;
    if (!(data == rhs.data))
      return false;
    return true;
  }
  bool operator != (const PendingRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const PendingRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(PendingRequest &a, PendingRequest &b);

inline std::ostream& operator<<(std::ostream& out, const PendingRequest& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _FriendRequest__isset {
  _FriendRequest__isset() : p_send_req(false), p_recv_req(false), greeting(false), time(false), id(false) {}
  bool p_send_req :1;
  bool p_recv_req :1;
  bool greeting :1;
  bool time :1;
  bool id :1;
} _FriendRequest__isset;

class FriendRequest {
 public:

  FriendRequest(const FriendRequest&);
  FriendRequest& operator=(const FriendRequest&);
  FriendRequest() : p_send_req(0), p_recv_req(0), greeting(), time(0), id(0) {
  }

  virtual ~FriendRequest() throw();
  int32_t p_send_req;
  int32_t p_recv_req;
  std::string greeting;
  int32_t time;
  int32_t id;

  _FriendRequest__isset __isset;

  void __set_p_send_req(const int32_t val);

  void __set_p_recv_req(const int32_t val);

  void __set_greeting(const std::string& val);

  void __set_time(const int32_t val);

  void __set_id(const int32_t val);

  bool operator == (const FriendRequest & rhs) const
  {
    if (!(p_send_req == rhs.p_send_req))
      return false;
    if (!(p_recv_req == rhs.p_recv_req))
      return false;
    if (!(greeting == rhs.greeting))
      return false;
    if (!(time == rhs.time))
      return false;
    if (!(id == rhs.id))
      return false;
    return true;
  }
  bool operator != (const FriendRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const FriendRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(FriendRequest &a, FriendRequest &b);

inline std::ostream& operator<<(std::ostream& out, const FriendRequest& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _FriendData__isset {
  _FriendData__isset() : id(false), name(false), isMale(false) {}
  bool id :1;
  bool name :1;
  bool isMale :1;
} _FriendData__isset;

class FriendData {
 public:

  FriendData(const FriendData&);
  FriendData& operator=(const FriendData&);
  FriendData() : id(0), name(), isMale(0) {
  }

  virtual ~FriendData() throw();
  int32_t id;
  std::string name;
  bool isMale;

  _FriendData__isset __isset;

  void __set_id(const int32_t val);

  void __set_name(const std::string& val);

  void __set_isMale(const bool val);

  bool operator == (const FriendData & rhs) const
  {
    if (!(id == rhs.id))
      return false;
    if (!(name == rhs.name))
      return false;
    if (!(isMale == rhs.isMale))
      return false;
    return true;
  }
  bool operator != (const FriendData &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const FriendData & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(FriendData &a, FriendData &b);

inline std::ostream& operator<<(std::ostream& out, const FriendData& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _pingResult__isset {
  _pingResult__isset() : haveData(false), data(false) {}
  bool haveData :1;
  bool data :1;
} _pingResult__isset;

class pingResult {
 public:

  pingResult(const pingResult&);
  pingResult& operator=(const pingResult&);
  pingResult() : haveData(0) {
  }

  virtual ~pingResult() throw();
  bool haveData;
  std::vector<FriendRequest>  data;

  _pingResult__isset __isset;

  void __set_haveData(const bool val);

  void __set_data(const std::vector<FriendRequest> & val);

  bool operator == (const pingResult & rhs) const
  {
    if (!(haveData == rhs.haveData))
      return false;
    if (!(data == rhs.data))
      return false;
    return true;
  }
  bool operator != (const pingResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const pingResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(pingResult &a, pingResult &b);

inline std::ostream& operator<<(std::ostream& out, const pingResult& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _listFriendResult__isset {
  _listFriendResult__isset() : size(false), idx(false), friendList(false), code(false) {}
  bool size :1;
  bool idx :1;
  bool friendList :1;
  bool code :1;
} _listFriendResult__isset;

class listFriendResult {
 public:

  listFriendResult(const listFriendResult&);
  listFriendResult& operator=(const listFriendResult&);
  listFriendResult() : size(0), idx(0), code((ErrorCode::type)0) {
  }

  virtual ~listFriendResult() throw();
  int32_t size;
  int32_t idx;
  std::vector<FriendData>  friendList;
  ErrorCode::type code;

  _listFriendResult__isset __isset;

  void __set_size(const int32_t val);

  void __set_idx(const int32_t val);

  void __set_friendList(const std::vector<FriendData> & val);

  void __set_code(const ErrorCode::type val);

  bool operator == (const listFriendResult & rhs) const
  {
    if (!(size == rhs.size))
      return false;
    if (!(idx == rhs.idx))
      return false;
    if (!(friendList == rhs.friendList))
      return false;
    if (!(code == rhs.code))
      return false;
    return true;
  }
  bool operator != (const listFriendResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const listFriendResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(listFriendResult &a, listFriendResult &b);

inline std::ostream& operator<<(std::ostream& out, const listFriendResult& obj)
{
  obj.printTo(out);
  return out;
}



#endif
