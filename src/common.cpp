#include "common.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif
#include <filesystem>

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
		assert(false);
	}

	return true;
}

#ifdef _WIN32
static std::filesystem::path project_folder_path_windows()
{
	std::string path;
	path.resize(MAX_PATH, 0);
	auto path_size(GetModuleFileName(nullptr, &(path.front()), MAX_PATH));
	std::filesystem::path p = path;
	std::cout << "Getting Windows Project Path: " << path << std::endl;
	return p.parent_path();
}
#endif

#ifdef __linux
// TODO: Add linux support, no ticket yet
static std::filesystem::path project_folder_path_linux()
{
	std::cout << "Getting Linux Asset Path: " << path << std::endl;
	return std::string(PROJECT_SOURCE_DIR);
}
#endif

#ifdef __APPLE__
static std::filesystem::path project_folder_path_mac_os()
{
	std::string path;
	char buf[PATH_MAX];
	uint32_t buf_size = PATH_MAX;

	if (!_NSGetExecutablePath(buf, &buf_size))
	{
		path = std::string(buf);
	}

	std::cout << "Getting OSX Asset Path: " << path << std::endl;
	return path;
}
#endif

std::string system_path(const std::string sub_path) {
	std::filesystem::path p;

	//return std::string(PROJECT_SOURCE_DIR) + "assets";
#ifdef _WIN32
	p = project_folder_path_windows();
#endif 
#ifdef __linux
	p = project_folder_path_linux();
#endif
#ifdef __APPLE__
	p = project_folder_path_mac_os();
#endif

	p = p.append(sub_path);
	std::string path = p.string();
	path.resize(p.string().size());
	return path;
};

std::string asset_path(const std::string sub_path) {
	std::string path;
	return system_path("assets/" + sub_path);
};

std::string shader_path(const std::string sub_path) {
	std::string path;
	return system_path("shaders/" + sub_path);
};
