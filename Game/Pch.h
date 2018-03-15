#include <Windows.h>
#undef CreateWindow
#undef DestroyWindow
#undef PlaySound

#define GLEW_STATIC
#include "sdks/glew/include/GL/glew.h"

#include "sdks/glfw/include/GLFW/glfw3.h"
#include "sdks/soil/include/SOIL.h"

#include "sdks/glm/glm.hpp"
#include "sdks/glm/gtc/matrix_transform.hpp"
#include "sdks/glm/gtc/type_ptr.hpp"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

#include "Engine/DebugPanels/imgui/imgui.h"
#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"

#include <map>
#include <string>
#include <sstream>
#include <vector>

void CheckGLError();
