# sorting algo visualization

a visualization thingy for various sorting algorithms

## Code

- written in/with C++ 20
- uses cmake (find_package or FetchContent)
- only tested under Windows 10 (2004) x64

## dependancies

- GUI: [Dear IMGUI](https://github.com/ocornut/imgui)
- Plotting: [ImPlot](https://github.com/epezent/implot)
- Windowing: [SFML](https://github.com/SFML/SFML)
- GUI binding: [ImGui-SFML](https://github.com/eliasdaler/imgui-sfml)
- Coroutines: [CppCoro](https://github.com/lewissbaker/cppcoro)
- Testing : [Catch2](https://github.com/catchorg/Catch2)

### changes made to dependancies

cppcoro/generator.hpp:36:83: note: must be declared with 'noexcept'
	constexpr std::experimental::suspend_always final_suspend() const { return {}; }

## build

avaiable cmake targets:
- one2III : user execuatable
- one2III-test : test executable

avaiable cmake options:
- ENABLE_CLANG_TIDY
  - may complain about disabled exceptions due to [LLVM Bug 45356](https://bugs.llvm.org/show_bug.cgi?id=45356)
  - replace all instances of ```/EHsc``` with ```-EHsc``` in build/ as a workaround
  - should be skipped for test target
