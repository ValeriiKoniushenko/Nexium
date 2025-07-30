
add_executable(TestEngine Main.cpp
        Graphics/ShaderManagerTest.cpp)

target_link_libraries(TestEngine PUBLIC
        gtest
        SpriteWalkerEngine
)
