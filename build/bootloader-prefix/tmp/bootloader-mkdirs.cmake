# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/PROG/esp-idf/ESP-IDF_container/esp-idf/components/bootloader/subproject"
  "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader"
  "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix"
  "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix/tmp"
  "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix/src/bootloader-stamp"
  "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix/src"
  "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/PROG/Portfolio/I2C-OLED-ETC/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
