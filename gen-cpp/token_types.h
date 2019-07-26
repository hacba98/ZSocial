/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef token_TYPES_H
#define token_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>




class token;

typedef struct _token__isset {
  _token__isset() : zuid(false) {}
  bool zuid :1;
} _token__isset;

class token {
 public:

  token(const token&);
  token& operator=(const token&);
  token() : zuid(0) {
  }

  virtual ~token() throw();
  int32_t zuid;

  _token__isset __isset;

  void __set_zuid(const int32_t val);

  bool operator == (const token & rhs) const
  {
    if (!(zuid == rhs.zuid))
      return false;
    return true;
  }
  bool operator != (const token &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const token & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(token &a, token &b);

inline std::ostream& operator<<(std::ostream& out, const token& obj)
{
  obj.printTo(out);
  return out;
}



#endif
