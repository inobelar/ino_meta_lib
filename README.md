# Inobelar's meta-programing library

The set of C++11 utility headers for doing compile-time magic.

# Examples

- [VariantEnum](./include/ino/meta/VariantEnum/VariantEnum.hpp)
- [SimplePacket](./include/ino/meta/SimplePacket/SimplePacket.hpp)

## Usage

Simply add `<path/to/this/repo>/include/` to your `include pathes`, for example:
- gcc:   `$ gcc -O2 ... -I <path/to/this/repo>/include/ ...`
- qmake: add the next into your `*.pro` file:
    - direct way: `INCLUDEPATH += <path/to/this/repo>/include/`
    - sub-project way: `include(<path/to/this/repo>/include/ino.pri)`
- cmake: [read on StackOverflow](https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake) 
## Notes

Add `INO_ENABLE_COMPILETIME_TESTS` define for enabling in-place 
compile-time tests - for debugging and hacking purpuse only, since it
may slow-down compilation.
