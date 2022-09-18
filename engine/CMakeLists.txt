file(GLOB base_src
  *.cpp
  *.h
  *.hpp

  api/*.cpp
  api/*.h
  api/*.hpp

  scene/*.cpp
  scene/*.h
  scene/*.hpp

  ../external/spirv_reflect/*.cpp
  ../external/spirv_reflect/*.h
  ../external/imgui/*.cpp
  ../external/imgui/*.h
  )


file(GLOB BASE_HEADERS
  *.h
  *.hpp

  api/*.h
  api/*.hpp

  scene/*.h
  scene/*.hpp

  ../external/spirv_reflect/*.h
  ../external/imgui/*.h
  )

add_library(engine STATIC ${base_src})

target_link_libraries(engine ${vulkan_lib} glfw ${xcb_libraries} ${cmake_thread_libs_init})