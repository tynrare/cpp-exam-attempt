### Installation ###
```bash
mkdir build
cd build
cmake .. && make
```

Tested on `Linux 4.17.17-1-MANJARO x86_64, cmake version 3.11.4`

### Usage ###

Available commands:

```
1-gen - generate 1gb file
1-parse - first test implementation
2-case - shared_mutex implementation
3-case - wildcards implementation
4-case - cache implementation
```

Example execute:

```
cd build
./CppTest 1-gen unsortedfile
./CppTest 1-parse unsortedfile sortedfile
```
