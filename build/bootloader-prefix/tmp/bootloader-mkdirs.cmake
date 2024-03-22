# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/ESP/container/esp-idf/components/bootloader/subproject"
  "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader"
  "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader-prefix"
  "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader-prefix/tmp"
  "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader-prefix/src/bootloader-stamp"
  "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader-prefix/src"
  "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/CacMonHoc/Nam3/NhungKhongDay/lab2/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
