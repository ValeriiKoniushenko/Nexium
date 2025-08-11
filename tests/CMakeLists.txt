
add_executable(TestEngine Main.cpp
    Graphics/ShaderManagerTest.cpp
    ECSTests.cpp
)

target_link_libraries(TestEngine PUBLIC
    gtest
    NexiumEngine
)
