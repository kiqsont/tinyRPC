// this file works for rpc data package

// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.28.1
// 	protoc        v3.21.12
// source: rpcHeader.proto

package User

import (
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	reflect "reflect"
	sync "sync"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

type RpcHeader struct {
	state         protoimpl.MessageState
	sizeCache     protoimpl.SizeCache
	unknownFields protoimpl.UnknownFields

	ServiceName []byte `protobuf:"bytes,1,opt,name=service_name,json=serviceName,proto3" json:"service_name,omitempty"`
	MethodName  []byte `protobuf:"bytes,2,opt,name=method_name,json=methodName,proto3" json:"method_name,omitempty"`
	ArgsSize    uint32 `protobuf:"varint,3,opt,name=args_size,json=argsSize,proto3" json:"args_size,omitempty"`
}

func (x *RpcHeader) Reset() {
	*x = RpcHeader{}
	if protoimpl.UnsafeEnabled {
		mi := &file_rpcHeader_proto_msgTypes[0]
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		ms.StoreMessageInfo(mi)
	}
}

func (x *RpcHeader) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*RpcHeader) ProtoMessage() {}

func (x *RpcHeader) ProtoReflect() protoreflect.Message {
	mi := &file_rpcHeader_proto_msgTypes[0]
	if protoimpl.UnsafeEnabled && x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use RpcHeader.ProtoReflect.Descriptor instead.
func (*RpcHeader) Descriptor() ([]byte, []int) {
	return file_rpcHeader_proto_rawDescGZIP(), []int{0}
}

func (x *RpcHeader) GetServiceName() []byte {
	if x != nil {
		return x.ServiceName
	}
	return nil
}

func (x *RpcHeader) GetMethodName() []byte {
	if x != nil {
		return x.MethodName
	}
	return nil
}

func (x *RpcHeader) GetArgsSize() uint32 {
	if x != nil {
		return x.ArgsSize
	}
	return 0
}

var File_rpcHeader_proto protoreflect.FileDescriptor

var file_rpcHeader_proto_rawDesc = []byte{
	0x0a, 0x0f, 0x72, 0x70, 0x63, 0x48, 0x65, 0x61, 0x64, 0x65, 0x72, 0x2e, 0x70, 0x72, 0x6f, 0x74,
	0x6f, 0x12, 0x07, 0x74, 0x69, 0x6e, 0x79, 0x52, 0x70, 0x63, 0x22, 0x6c, 0x0a, 0x09, 0x52, 0x70,
	0x63, 0x48, 0x65, 0x61, 0x64, 0x65, 0x72, 0x12, 0x21, 0x0a, 0x0c, 0x73, 0x65, 0x72, 0x76, 0x69,
	0x63, 0x65, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x18, 0x01, 0x20, 0x01, 0x28, 0x0c, 0x52, 0x0b, 0x73,
	0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x4e, 0x61, 0x6d, 0x65, 0x12, 0x1f, 0x0a, 0x0b, 0x6d, 0x65,
	0x74, 0x68, 0x6f, 0x64, 0x5f, 0x6e, 0x61, 0x6d, 0x65, 0x18, 0x02, 0x20, 0x01, 0x28, 0x0c, 0x52,
	0x0a, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x4e, 0x61, 0x6d, 0x65, 0x12, 0x1b, 0x0a, 0x09, 0x61,
	0x72, 0x67, 0x73, 0x5f, 0x73, 0x69, 0x7a, 0x65, 0x18, 0x03, 0x20, 0x01, 0x28, 0x0d, 0x52, 0x08,
	0x61, 0x72, 0x67, 0x73, 0x53, 0x69, 0x7a, 0x65, 0x42, 0x08, 0x5a, 0x06, 0x2e, 0x2f, 0x55, 0x73,
	0x65, 0x72, 0x62, 0x06, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x33,
}

var (
	file_rpcHeader_proto_rawDescOnce sync.Once
	file_rpcHeader_proto_rawDescData = file_rpcHeader_proto_rawDesc
)

func file_rpcHeader_proto_rawDescGZIP() []byte {
	file_rpcHeader_proto_rawDescOnce.Do(func() {
		file_rpcHeader_proto_rawDescData = protoimpl.X.CompressGZIP(file_rpcHeader_proto_rawDescData)
	})
	return file_rpcHeader_proto_rawDescData
}

var file_rpcHeader_proto_msgTypes = make([]protoimpl.MessageInfo, 1)
var file_rpcHeader_proto_goTypes = []interface{}{
	(*RpcHeader)(nil), // 0: tinyRpc.RpcHeader
}
var file_rpcHeader_proto_depIdxs = []int32{
	0, // [0:0] is the sub-list for method output_type
	0, // [0:0] is the sub-list for method input_type
	0, // [0:0] is the sub-list for extension type_name
	0, // [0:0] is the sub-list for extension extendee
	0, // [0:0] is the sub-list for field type_name
}

func init() { file_rpcHeader_proto_init() }
func file_rpcHeader_proto_init() {
	if File_rpcHeader_proto != nil {
		return
	}
	if !protoimpl.UnsafeEnabled {
		file_rpcHeader_proto_msgTypes[0].Exporter = func(v interface{}, i int) interface{} {
			switch v := v.(*RpcHeader); i {
			case 0:
				return &v.state
			case 1:
				return &v.sizeCache
			case 2:
				return &v.unknownFields
			default:
				return nil
			}
		}
	}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: file_rpcHeader_proto_rawDesc,
			NumEnums:      0,
			NumMessages:   1,
			NumExtensions: 0,
			NumServices:   0,
		},
		GoTypes:           file_rpcHeader_proto_goTypes,
		DependencyIndexes: file_rpcHeader_proto_depIdxs,
		MessageInfos:      file_rpcHeader_proto_msgTypes,
	}.Build()
	File_rpcHeader_proto = out.File
	file_rpcHeader_proto_rawDesc = nil
	file_rpcHeader_proto_goTypes = nil
	file_rpcHeader_proto_depIdxs = nil
}
