include (FetchContent)

set_property (DIRECTORY PROPERTY EP_BASE dependencies)

set (DEPENDENCIES)
set (EXTRA_CMAKE_ARGS)

FetchContent_Declare(Utils
        GIT_REPOSITORY  git@github.com:ValeriiKoniushenko/Utils.git
        GIT_TAG         origin/develop
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(Utils)



FetchContent_Declare(Glad
        GIT_REPOSITORY  git@github.com:Dav1dde/glad.git
        GIT_TAG         v2.0.8
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(Glad)



FetchContent_Declare(Glfw
        GIT_REPOSITORY  git@github.com:glfw/glfw.git
        GIT_TAG         3.4
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(Glad)



FetchContent_Declare(QtBase
        GIT_REPOSITORY  git@github.com:qt/qtbase.git
        GIT_TAG         v6.9.1
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(QtBase)

