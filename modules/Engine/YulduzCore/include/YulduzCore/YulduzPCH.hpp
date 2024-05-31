#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdarg>
#include <cstdio>
#include <iomanip>

#include <vector>
#include <span>
#include <string>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <chrono>
#include <memory>
#include <thread>
#include <format>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <wgpu/wgpu_impl_glfw.h>
#include <wgpu/wgpu.h>

#if defined(_WIN32) || defined(_WIN64)
#define YULDUZ_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#define YULDUZ_PLATFORM_OSX
#define YULDUZ_PLATFORM_UNIX
#elif defined(__linux__)
#define YULDUZ_PLATFORM_LINUX
#define YULDUZ_PLATFORM_UNIX
#else
#error Not supported platform
#endif

#ifndef YULDUZ_SOURCE_DIR
#define YULDUZ_SOURCE_DIR ""
#endif

#ifndef YULDUZ_BUILD_TYPE_RELEASE
#define YULDUZ_BUILD_TYPE_DEBUG
#endif