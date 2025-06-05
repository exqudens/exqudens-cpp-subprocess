# exqudens-cpp-subprocess

##### how-to-export-all-packages

1. `conan remove -c 'exqudens-cpp-subprocess'`
2. `git clean -xdf`
3. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --preset {} || exit 255"`
4. `cmake --list-presets | cut -d ':' -f2 | xargs -I '{}' echo '{}' | xargs -I '{}' bash -c "cmake --build --preset {} --target conan-export || exit 255"`
