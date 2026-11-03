add_library(Nexium_Platform_Input STATIC)
add_library(Nexium::Platform::Input ALIAS Nexium_Platform_Input)
target_sources(Nexium_Platform_Input
	PRIVATE
		Window.cpp
		Events/InputEvent.h
		Events/MouseEvents.h
		Events/WindowEvents.h
		Events/GamepadEvents.h
		Events/KeyboardEvents.h
		InputDevices/Mouse.cpp
		InputDevices/Joystick.cpp
		InputDevices/Keyboard.cpp
)
nexium_add_public_headers(
	Nexium_Platform_Input
	Platform/Input/Glfw.h
	Platform/Input/Window.h
	Platform/Input/Window.generated.h
	Platform/Input/InputDevices/Mouse.h
	Platform/Input/InputDevices/Keyboard.h
	Platform/Input/InputDevices/Joystick.h
	Platform/Input/InputDevices/Mouse.generated.h
	Platform/Input/InputDevices/Keyboard.generated.h
)

target_link_libraries(Nexium_Platform_Input
	PUBLIC
		Nexium::Foundation::Base
		Nexium::Foundation::Interfaces
		JustReflectMe::Adapter
		Utils::Core
		glfw
		glm
	PRIVATE
		Core::Stb
		Nexium::Platform::Graphics
		Nexium::Platform::Resources
		${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_Platform_Input)
add_dependencies(Nexium_Platform_Input CodeGenerator)
