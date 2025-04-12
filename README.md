# Learning Automotive Software & Architecture

Exploring Service-Oriented Architecture (SOA) architecture for vehicles.

## Dependencies (Linux)

For C++ services

```bash
sudo apt update
sudo apt install -y build-essential cmake
sudo apt install -y libgrpc++-dev libgrpc-dev
sudo apt install -y protobuf-compiler protobuf-compiler-grpc libprotobuf-dev
```

For Go client

```bash
sudo apt update
sudo apt install golang-go
sudo apt install -y protobuf-compiler
```

### Required Go packages

``` bash
go install google.golang.org/protobuf/cmd/protoc-gen-go@latest
go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest
```

Add the Go bin directory to your PATH if it's not already there

```bash
export PATH="$PATH:$(go env GOPATH)/bin"
```

Install gRPC package

```bash
go get google.golang.org/grpc
```

## Build

Build the obd gRPC server using CMake:

```bash
cd obd_service
rm -rf build
mkdir build
cd build
cmake ..
make
```

Generate Go code from proto file and buil the go client

```bash
go build -o obd_client
protoc --proto_path=../proto --go_out=. --go_opt=paths=source_relative \
    --go-grpc_out=. --go-grpc_opt=paths=source_relative obd_service.proto
go build -o obd_client
```

## Run

1. Make sure your C++ gRPC server is running:
    
    ```bash
    cd obd_service/build
    ./obd_server
    ```
    
2. In another terminal, run the Go client:
    
    ```bash
    cd obd_go_client
    ./obd_client
    ```
    
The Go client should connect to the C++ server and perform the same operations:

- Get a single fuel level reading
- Stream fuel level updates for a few seconds
