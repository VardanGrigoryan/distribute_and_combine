# A toy, just-for-fun text analyzer project based on distribute and combine pattern

## Introduction
Text analyzer is a scalable, offline, multithreaded software solution developed in a library-like, headers-only maner which is capable to process huge files and extract the required information even when the input file is bigger that the ram memory available on the processing machine.
Currently it supports only two types of infomation retrieval:
- words and their appropriate frequencies
- smileys and their global positions in the original text

### High level algorithm
Text analyzer is using distribute and combine strategy. It processes an input file by chunks which is configurable, so one can try a different values for the chunk sizes. Each read chunk of text is distributing to separate thread by this by this parallelizing the overall process. At the same time it is also combining the processed data, so reading and processing are almost going in parallel. When thread completes a task the results can be keeped in two ways in ram-memory or in persistend disk. In the later case, the overall process will be slightly slower as multiple database queries are taking place, but one the other hand it is capable to process huge files. When the input file is smaller then database usage can by bypassed.

After having all the results combined it generates an output statistics. Currently there are three types of it:
- xml file
- text file
- console representation

## Tech stack and dependencies

Text analyzer uses the following stack:

- C++
- bash script
- CMake
- Sqlite3
- boost
- boost unit test module
- stl
- Docker - ?

## Environment
As a build system CMake has been used and was written so it could be compiled and executed on linux and windows environments.

## Build
Dockerfile is provided so one can build and execute the project on a totaly isolated environmrnt. Here is an issue ralted with os image version that's not fixed inthis initial version!
In order to build the project manually, please follow to bellow described steps:
```
$ cd [project directory]
$ ./build.sh -o [operating system] -t [debug/release] -e [enable tests y/n] -c [clean build y/n]
```
After having this done the executable file could be found in the ```./bin``` area of current project root directory.

## Usage
```
Usage: ./bin/analyze_statistics -c [chunk size] -i [input_file_path] -d [db_path] -n [top] -f [output format] -o [output_file_path]
Arguments descriptions:
	-i | input_file_path, The Input file path
	-n | top, Gets n most frequent words
	-f | output_format, supported formats [xml | file | console], Indicates in which format to represent the output

Optional Arguments:
	-c | chunk_size, Indicates in which portions the input text file should be processed
	-d | db_path, Indicates the database name if it is going to be used
	-o | output_file_path, The output file path
```

## Tests
Boost unit test framework has been used for tests development. Currently there are 16 tests which is by far less than a full coverage. More than a hundred unit tests are required to develop in order to guarantee at max 60% of overall coverage.

In order to run the unit test:
```
$ ./bin/analyze_statistics_unit_tests
```
