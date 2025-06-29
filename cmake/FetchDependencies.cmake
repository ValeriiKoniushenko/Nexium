include (FetchContent)

set_property (DIRECTORY PROPERTY EP_BASE dependencies)

set (DEPENDENCIES)
set (EXTRA_CMAKE_ARGS)

FetchContent_Declare(Utils
    GIT_REPOSITORY git@github.com:ValeriiKoniushenko/Utils.git
    GIT_TAG origin/develop
)
FetchContent_MakeAvailable(Utils)