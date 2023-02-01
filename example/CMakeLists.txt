add_executable(callee1 calleeUser.cc proto/User.pb.cc)
target_link_libraries(callee1 tinyRPC protobuf)

add_executable(caller1 callerUser.cc proto/User.pb.cc)
target_link_libraries(caller1 tinyRPC protobuf)

add_executable(callee2 calleeFriends.cc proto/Friends.pb.cc)
target_link_libraries(callee2 tinyRPC protobuf)

add_executable(caller2 callerFriends.cc proto/Friends.pb.cc)
target_link_libraries(caller2 tinyRPC protobuf)

add_executable(load loadConfig.cc)
target_link_libraries(load tinyRPC)

add_executable(threadpool ThreadPoolTesting.cc)
target_link_libraries(threadpool tinyRPC)