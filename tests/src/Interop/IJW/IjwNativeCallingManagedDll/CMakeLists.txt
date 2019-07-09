cmake_minimum_required (VERSION 3.12)
project (IjwNativeCallingManagedDll)

include_directories( ${INC_PLATFORM_DIR} )
set(SOURCES IjwNativeCallingManagedDll.cpp)

# add the shared library
add_library (IjwNativeCallingManagedDll SHARED ${SOURCES})
target_link_libraries(IjwNativeCallingManagedDll ${LINK_LIBRARIES_ADDITIONAL})
set_target_properties(IjwNativeCallingManagedDll PROPERTIES COMMON_LANGUAGE_RUNTIME netcore)

# add the install targets
install (TARGETS IjwNativeCallingManagedDll DESTINATION bin)
