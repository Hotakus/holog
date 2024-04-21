<center>
  <img src="./assets/holog_banner.png" title="" alt="banner" data-align="center">
<h3 align="center">Holog</h3>
</center>

<div align="center">
<strong>
      A highly customizable, low-coupling, multi-platform, user-friendly logging project  <br>
    specifically designed for embedded platforms.
</strong>
</div>

---

### Features：
- 🔗 Publish-Subscribe Pattern, support multi-devices(abstract logger object)
- 🖥️ Highly Customizable, user can customize styles of log easily.
- 🌍 Embedded Platform and multi-system support, Such as FatFs, LittleFS, windows and linux.
- 🛠️ Support standard output, also compatible with Serial port(need a little of porting).
- 🚀 Easy-to-use, just need follow example, you can start quickly.

---

### 🚀 Quickly Start：

First, you need pull this repo, use SSH:  
```shell
git clone --recurse-submodules git@github.com:Hotakus/holog.git
cd holog
```

Now you can use build.sh to build this project, or use your CMakeLists.txt to import this project:

**1️⃣ Use "CMakeLists.txt" to import:**  
in your project(not this project), write below:
```cmake
# include ".h" file
include_directories(./your_holog_path/holog)
# add holog as subproject
add_subdirectory(./your_holog_path/holog)

add_executable(your_project_exe ${YOUR_SRCS})
# below the "add_executable()", add holog as static lib
target_link_libraries(your_project_exe holog)
```
now, rebuild your project, the holog will be compiled with your project.

**2️⃣ Or use "build.sh":**  
In the root of HoLog, in shell:
```shell
# no parameters, generate 'holog' directory, include holog.a and all header files 
./build.sh

# with '-D' parameters
# -DHOLOG_USE_TEST : generate holog executable file in ./bin for test
./build.sh -DHOLOG_USE_TEST
```

---

### ⌨️ Code Eaxmple


  a  
  a  
  a  
    a  
a  
  aa  
  a  
  a  
  a  
