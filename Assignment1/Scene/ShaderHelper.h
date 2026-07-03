/**
 * ShaderHelper.h
 *
 * Reusable shader compilation/linking helper used by all Model subclasses.
 * Provides two loading paths:
 *   Method 1 – "stringify": build a program directly from inline C-string source.
 *   Method 2 – file-based: load .glsl source from disk (Desktop/Web) or APK
 *               assets (Android), then funnel into Method 1.
 *
 * All compile/link logic lives here once; no duplication across shape files.
 */

#pragma once

#include "Platform.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

class ShaderHelper {
public:
    // -----------------------------------------------------------------------
    // Core compile/link — shared by both loading methods below
    // -----------------------------------------------------------------------
    static GLuint compileShader(GLenum type, const char* src)
    {
        GLuint shader = glCreateShader(type);
        if (!shader) { LOGE("ShaderHelper: glCreateShader failed (type=0x%x)", type); return 0; }
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            GLint len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            std::string log(static_cast<size_t>(len), '\0');
            glGetShaderInfoLog(shader, len, nullptr, &log[0]);
            LOGE("ShaderHelper: shader compile error:\n%s", log.c_str());
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    static GLuint linkProgram(GLuint vert, GLuint frag)
    {
        GLuint prog = glCreateProgram();
        glAttachShader(prog, vert);
        glAttachShader(prog, frag);
        glLinkProgram(prog);
        GLint ok = GL_FALSE;
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (!ok) {
            GLint len = 0;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
            std::string log(static_cast<size_t>(len), '\0');
            glGetProgramInfoLog(prog, len, nullptr, &log[0]);
            LOGE("ShaderHelper: program link error:\n%s", log.c_str());
            glDeleteProgram(prog);
            prog = 0;
        }
        glDetachShader(prog, vert);
        glDetachShader(prog, frag);
        glDeleteShader(vert);
        glDeleteShader(frag);
        return prog;
    }

    // -----------------------------------------------------------------------
    // Method 1: "stringify" — build directly from inline source strings
    // Available for any future Model that prefers to embed shader source in code.
    // -----------------------------------------------------------------------
    static GLuint buildProgram(const char* vertSrc, const char* fragSrc)
    {
        GLuint vs = compileShader(GL_VERTEX_SHADER,   vertSrc);
        if (!vs) return 0;
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
        if (!fs) { glDeleteShader(vs); return 0; }
        return linkProgram(vs, fs);
    }

    // -----------------------------------------------------------------------
    // Method 2: file-based — load shader source from disk / APK assets
    // -----------------------------------------------------------------------
#ifdef PLATFORM_ANDROID

    static std::string loadAsset(AAssetManager* mgr, const char* path)
    {
        AAsset* asset = AAssetManager_open(mgr, path, AASSET_MODE_BUFFER);
        if (!asset) {
            LOGE("ShaderHelper: AAssetManager_open failed for '%s'", path);
            return {};
        }
        size_t size = static_cast<size_t>(AAsset_getLength(asset));
        std::string src(size, '\0');
        AAsset_read(asset, &src[0], size);
        AAsset_close(asset);
        return src;
    }

    static GLuint buildProgramFromAssets(AAssetManager* mgr,
                                         const char* vertPath,
                                         const char* fragPath)
    {
        std::string vertSrc = loadAsset(mgr, vertPath);
        std::string fragSrc = loadAsset(mgr, fragPath);
        if (vertSrc.empty() || fragSrc.empty()) {
            LOGE("ShaderHelper: failed to load shader assets (%s, %s)", vertPath, fragPath);
            return 0;
        }
        return buildProgram(vertSrc.c_str(), fragSrc.c_str());
    }

#else // Desktop / Web

    // Try paths in this order so the common post-build layout resolves first,
    // with fallbacks for running the executable from unusual working directories.
    static std::string loadFile(const char* filename)
    {
        const char* prefixes[] = {
            "assets/shader/",
            "",
            "shader/",
            "assets/"
        };
        for (const char* prefix : prefixes) {
            std::string path = std::string(prefix) + filename;
            std::ifstream f(path);
            if (f.good()) {
                std::ostringstream ss;
                ss << f.rdbuf();
                LOGI("ShaderHelper: loaded shader '%s'", path.c_str());
                return ss.str();
            }
        }
        LOGE("ShaderHelper: could not find shader file '%s' in any search path", filename);
        return {};
    }

    static GLuint buildProgramFromFile(const char* vertFile, const char* fragFile)
    {
        std::string vertSrc = loadFile(vertFile);
        std::string fragSrc = loadFile(fragFile);
        if (vertSrc.empty() || fragSrc.empty()) {
            LOGE("ShaderHelper: failed to load shader files (%s, %s)", vertFile, fragFile);
            return 0;
        }
        return buildProgram(vertSrc.c_str(), fragSrc.c_str());
    }

#endif // PLATFORM_ANDROID
};
