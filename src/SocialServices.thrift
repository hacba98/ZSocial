/**
	Thrift file define services for friend function
	Project Friend Services
*/

// -- Define error codes return by services
enum ErrorCode {
	SUCCESS = 0,
	USER_NOT_FOUND = 1,
	INVALID_PARAMETER = 2,
	INVALID_SERVICE = 3,
	INTERNAL_ERROR = 4,
	DUPLICATED_REQUEST = 5,
	OUT_OF_RANGE = 6
}

// USER PROFILE STRUCT
struct UserProfile{
	1:i32 id,	
	2:string name,
	3:i32 birth,
	4:bool gender,
	5:string username,
	6:string password,
	7:i64 phoneNumber,
	// join date
	// last active time
}

struct SimpleProfile {
	1:i32 id,
	2:string name,
	3:bool gender,
	// last active time
}

struct CreateUserResult {
	1:ErrorCode errorCode,
	2:i32 id
}

struct GetUserResult{
	1:ErrorCode errorCode,
	2:UserProfile profile
}

struct ListProfileResult{
	1:ErrorCode errorCode,
	2:list<SimpleProfile> profiles
}

// FRIEND STRUCT
struct FriendList {
	1: i32 size,
	2: set<i32> data
}

struct PendingRequest {
	1: i32 size,
	2: set<i32> data
}

struct FriendRequest {
	1: i32 p_send_req, // id of person who sending friend request
	2: i32 p_recv_req, // vice versa
	3: string greeting,
	4: i32 time,
	5: i32 id
}

struct FriendData { // simplify version of User - hinding sensitive data
	1: i32 id,
	2: string name,
	3: bool isMale
}

////////////////////////////////////////////////////////
//		Services exposed by server            //
////////////////////////////////////////////////////////



// Define data structure for services

// -- Structure server send to return user information
struct pingResult {
	1: bool haveData,
	2: list<FriendRequest> data
}

struct listFriendResult {
	// size of result
	1: i32 size,
	// idx of next item in list, -1 if it is last page
	2: i32 idx,
	3: list<FriendData> friendList,
	4: ErrorCode code
}

service ProfileServices {
	// API For Proifle

	// Login / Logout
	i32 Login(1:string username, 2:string password),
	ErrorCode Logout(1:i32 userId), 

	// Create new user
	CreateUserResult CreateProfile(1:UserProfile profile),
	
	// Get User Profile
	GetUserResult GetProfile(1:i32 userId),

	// Update User Profile
	ErrorCode UpdateProfile(1:UserProfile profile, 2:i32 userId),

	// Delete User Profile
	ErrorCode DeleteProfile(1:i32 userId),

	// Create new user
	CreateUserResult ansyCreateProfile(1:UserProfile profile),
	
	// Update User Profile
	ErrorCode ansyUpdateProfile(1:UserProfile profile, 2:i32 userId),

	// Delete User Profile
	ErrorCode ansyDeleteProfile(1:i32 userId),

	// Get List of profiles
	ListProfileResult getList(1:list<i32> ids),

	// Get UserId from username
	i32 GetIdByName(1:string username),
}

service FriendServices {

	// -- API for friend function
	// Check for new friend request
	pingResult checkRequest(1: i32 id);

	// Add new friend -> Async req
	ErrorCode addFriend(1: FriendRequest request);
	
	// Accept request
	ErrorCode acceptRequest(1: i32 curId, 2: i32 requestId);

	// Decline request
	ErrorCode declineRequest(1: i32 curId, 2: i32 requestId);

	// Remove a friend
	ErrorCode removeFriend(1: i32 curId, 2: i32 friendId);

	// View friend list
	listFriendResult viewFriendList (1: i32 id, 2: i32 index, 3: i32 size);
}

