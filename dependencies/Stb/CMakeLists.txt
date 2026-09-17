
add_library(Core_Stb STATIC Image.h Image.cpp)

add_library(Core::Stb ALIAS Core_Stb)

target_include_directories(Core_Stb PUBLIC ../)