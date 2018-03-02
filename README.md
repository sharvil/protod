# protod
A dependency analyzer for [Protocol Buffer](https://github.com/google/protobuf) files.

This tool generates the transitive closure of imports in the protobuf files you specify. It prints out a simple list, which is easy to manipulate on the command line.

## Building and installing
You'll need `libprotobuf` (C++ runtime) installed on your machine to build `protod`. You can either use your distribution's package manager to install it (e.g. apt), or you can install it from source by following the instructions on their [project page](https://github.com/google/protobuf/tree/master/src).

`protod` installs in `/usr/local` by default. If you want to change where it gets installed, edit `PREFIX` in the Makefile. Run the usual make commands to build and install:

```
$ make && make install
```

## Examples
#### Find all dependencies of a .proto file
```
$ protod src/proto/login.proto
```

#### Importing minimal dependencies
If you only want to import the Speech v1 API from [Google APIs](https://github.com/googleapis/googleapis) into your project, you can run `protod` like so:

```
$ cd /path/to/googleapis
$ rsync -R $(protod google/cloud/speech/v1/cloud_speech.proto | grep -v /usr) /path/to/project/proto
```

## License
Apache 2.0
