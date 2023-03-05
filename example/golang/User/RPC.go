package User

import (
	"encoding/binary"
	"fmt"
	"net"

	"google.golang.org/protobuf/proto"
)

// golang版的没有封装框架，所以只采用最原始的rpc
func Test() {
	req := LoginRequest{
		Username: []byte("kiqsont"),
		Password: []byte("K-Passowrd"),
	}
	args_str, _ := proto.Marshal(&req)

	rpcHeader := RpcHeader{
		ServiceName: []byte("UserServiceRPC"),
		MethodName:  []byte("Login"),
		ArgsSize:    uint32(len(args_str)),
	}
	// rpc_header_str := rpcHeader.String()
	rpc_header_str, _ := proto.Marshal(&rpcHeader)

	header_size := uint32(len(rpc_header_str))
	header_buf := make([]byte, 4)
	binary.LittleEndian.PutUint32(header_buf, header_size)
	pack := append(header_buf, rpc_header_str...)
	pack = append(pack, args_str...)

	conn, err := net.Dial("tcp4", "127.0.0.1:8888")
	if err != nil {
		panic(err)
	}

	conn.Write([]byte(pack))
	buf := make([]byte, 512)
	cnt, err := conn.Read(buf)
	if err != nil {
		panic(err)
	}

	resp := new(LoginResponse)
	proto.Unmarshal(buf[:cnt], resp)
	if resp.GetSuccess() {
		fmt.Println("Login")
	} else {
		fmt.Println("Failed to login")
	}
	fmt.Println("errInfo code:", resp.GetErrInfo().GetCode(), " errInfo msg:", resp.GetErrInfo().GetErrmsg())
}
