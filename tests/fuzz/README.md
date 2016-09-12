# Fuzz Testing

To run the fuzz tests, you'll first need to install [AFL](http://lcamtuf.coredump.cx/afl/).

Then to build and run a test, you'll need to do something like this from the project root:

```
echo 'using darwin : afl : afl-clang ;' > project-config.jam
cd tests/fuzz/URL
b2 toolset=darwin-afl linkflags=-lc++
afl-fuzz -i inputs -o findings bin/darwin-afl/debug/test @@
```

You might also want to read about AFL's higher-performance LLVM mode for faster tests.
