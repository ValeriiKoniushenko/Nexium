
add_executable(TestEngine Main.cpp
        RawGraphics/ShaderManagerTest.cpp)

target_link_libraries(TestEngine PUBLIC
        gtest
        SpriteWalkerEngine
)
