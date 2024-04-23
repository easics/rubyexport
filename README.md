<!--- This file is part of rubyexport. -->
<!---  -->
<!--- rubyexport is free software: you can redistribute it and/or modify it under -->
<!--- the terms of the GNU General Public License as published by the Free Software -->
<!--- Foundation, either version 3 of the License, or (at your option) any later -->
<!--- version. -->
<!---  -->
<!--- rubyexport is distributed in the hope that it will be useful, but WITHOUT ANY -->
<!--- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A -->
<!--- PARTICULAR PURPOSE. See the GNU General Public License for more details. -->
<!---  -->
<!--- You should have received a copy of the GNU General Public License along with -->
<!--- rubyexport. If not, see <https://www.gnu.org/licenses/>. -->

# Rubyexport

A library to export C++ files to be used in Ruby or Python scripts

**Warning**
We do not support Ruby version 2.7.x

## Compilation

Compilation is done with [CMake](https://cmake.org/)

Generate the project build system:

```bash
cmake -B build -S ./ -D SCRIPT_RUBY=ON -D SCRIPT_PYTHON=ON
```

You must define either to build for RUBY script support, Python script support
or both

By default the build system will use Makefiles.
To use Ninja files you can run the following command:

```bash
cmake -B build -S ./ -G Ninja -D SCRIPT_RUBY=ON -D SCRIPT_PYTHON=ON
```

Build the project:

```bash
cmake --build build
```

To specify the maximum number of concurrent processes when building, you can use
the following option:

```bash
cmake --build build -j 4
```

## Packages

For Ubuntu and Debian the following packages are required:
* cmake
* For Ninja builds: build-essential
* python3-dev
* ruby-dev

## License

rubyexport is distributed under the
[GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

## Contact info

Created by [easics](https://www.easics.com/)

Tool specific questions: tools@easics.be
