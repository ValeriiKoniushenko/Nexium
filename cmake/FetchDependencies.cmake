include (FetchContent)

set_property (DIRECTORY PROPERTY EP_BASE dependencies)

FetchContent_Declare(Utils
        GIT_REPOSITORY  git@github.com:ValeriiKoniushenko/Utils.git
        GIT_TAG         origin/develop
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(Utils)



FetchContent_Declare(Glfw
        GIT_REPOSITORY  git@github.com:glfw/glfw.git
        GIT_TAG         3.4
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(Glfw)



FetchContent_Declare(BetterEnums
        GIT_REPOSITORY  git@github.com:aantron/better-enums.git
        GIT_TAG         0.11.3
        GIT_SHALLOW     TRUE          # Enables --depth 1
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(BetterEnums)



# FetchContent_Declare(QtBase
#         GIT_REPOSITORY  git@github.com:qt/qtbase.git
#         GIT_TAG         v6.9.1
#         GIT_SHALLOW     TRUE          # Enables --depth 1
#         GIT_PROGRESS    TRUE
# )
# FetchContent_MakeAvailable(QtBase)

