# AndroidProgrammingQuiz-CrossPlatformCI

Cross-platform OpenGL ES renderer with a Model + Renderer architecture, building
on Tutorial 4 (3-HelloWorldOpenGLES-JNI-Android-PC-WebGL-CrossPlatform).

Renders a rotating RGB triangle alongside a yellow square on all three targets:
Desktop (Windows/GLFW), Android, and Web (Emscripten/WebGL).

## Project Structure

```
Scene/
  Platform.h       - GL + windowing headers, LOGI/LOGE macros
  Model.h          - Abstract base class: InitModel / Render / Resize
  ShaderHelper.h   - Reusable shader compile/link, file-based and inline paths
  Renderer.h/.cpp  - Singleton controller owning all Model instances
  Triangle.h/.cpp  - Concrete Model: rotating RGB triangle
  Square.h/.cpp    - Concrete Model: static yellow square (offset right)
  main.cpp         - Entry point for Desktop and Web builds
android/
  app/src/main/cpp/NativeTemplate.cpp  - JNI bridge to Renderer
  app/src/main/cpp/CMakeLists.txt      - Android build config
  app/src/main/assets/shader/          - GLSL shader files (all platforms)
CMakeLists.txt       - Desktop build
build_web.bat/.sh    - Web (Emscripten) build
```

## How to Build

### Desktop (Windows)

```bat
cmake -S . -B build_desktop -G "Visual Studio 17 2022" -A x64
cmake --build build_desktop --config Debug
.\build_desktop\Debug\GLPIFrameworkIntro.exe
```

Or use the convenience script:
```bat
script_build_and_run.bat
```

Press **ESC** to close the window.

### Android

1. Open the `android/` folder in Android Studio.
2. Let Gradle sync.
3. Run on a device or emulator (API 24+, OpenGL ES 3.0 required).

### Web (Emscripten)

Ensure `emcc` is on your PATH (run `emsdk_env.bat` first), then:

```bat
build_web.bat
cd web
python -m http.server 8080
```

Open `http://localhost:8080/` in a browser.
Check the browser console for the `[INFO] GLPI Framework Intro - WebGL` log line.

## Clean Rebuild

- Desktop: delete `build_desktop/` and re-run cmake.
- Android: Gradle → "Clean Project", then rebuild.
- Web: delete `web/` and re-run `build_web.bat`.
