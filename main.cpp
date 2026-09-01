#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <bits/pthread_types.h>
#include <cstddef>
#include <cstdint>
#include <dlfcn.h>
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_android.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "Dobby/dobby.h"
#include "ByNameModding/Includes.h"
#include "ByNameModding/fake_dlfcn.h"
#include "ByNameModding/Il2Cpp.h"


#include <pthread.h>
#include <jni.h>
#include <sys/cdefs.h>
#include <unistd.h>
#include "Viscount/memory.h"

uintptr_t base = 0;

bool clearMousePos = true, setup = false;
struct UnityEngine_Vector2_Fields {
    float x;
    float y;
};

struct UnityEngine_Vector2_o {
    UnityEngine_Vector2_Fields fields;
};

enum TouchPhase {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4
};




struct UnityEngine_Touch_Fields {
    int32_t m_FingerId;
    struct UnityEngine_Vector2_o m_Position;
    struct UnityEngine_Vector2_o m_RawPosition;
    struct UnityEngine_Vector2_o m_PositionDelta;
    float m_TimeDelta;
    int32_t m_TapCount;
    int32_t m_Phase;
    int32_t m_Type;
    float m_Pressure;
    float m_maximumPossiblePressure;
    float m_Radius;
    float m_fRadiusVariance;
    float m_AltitudeAngle;
    float m_AzimuthAngle;
};
bool debug = false;
bool shop = false;
bool jump = false;
bool no_dealth = false;
bool stopTrain = false;
float speed = 0.0f;
float speed1 = 0.0f;
bool patch = false;
bool patch1 = false;
// bool (*original)(void *instance);
// bool origin_call(void *instance) {
//     if (shop) {
//         return true;
//     }
//     return original(instance);
// }
// bool (*old_jump)(void *instance);
// bool get_jump(void *instance) {
//     if (jump) {
//         return true;
//     }
//     return old_jump(instance);
// }
bool (*original_no_front)(void* instance, void* idk);
bool no_front(void* instance, void* idk) {
    if (no_dealth) {
        return false;
    }
    return original_no_front(instance, idk);
}
bool (*original_no_side)(void* instance, void* idk);
bool no_side(void* instance, void* idk) {
    if (no_dealth) {
        return false;
    }
    return original_no_side(instance, idk);
}
bool (*original_camera)(void* instance, void* idk);
bool follow_camera(void* instance, void* idk) {
    if (instance != NULL) {
        return true;
    }
    return original_camera(instance, idk);
}
bool (*old_train)(void* instance);
bool train(void* instance) {
    if (stopTrain) {
        return true;
    }
    return old_train(instance);
}
float (*original_speed)(void* instance);
float getMinSpeed(void* instance) {
    if (speed > 0.0f) {
        return speed;
    }
    return original_speed(instance);
}
float (*original_speed1)(void* instance);
float getMinSpeed1(void* instance) {
    if (speed1 > 0.0f) {
        return speed1;
    }
    return original_speed1(instance);
}
void hack() {
    void* shop = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway.Core.CommonData", "Currency", "get_IsIAP", 0);
    // DobbyHook(shop, (void *)origin_call, (void **)&original);
    void* jump_off = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.RunnerCore.Character", "CharacterMotor", "get_CanJump", 0);
    // DobbyHook(jump_off, (void *)get_jump, (void **)&old_jump);
    void* front_off = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.RunnerCore.Character", "CharacterMotor", "CheckFrontalImpact", 1);
    DobbyHook(front_off, (void *)no_front, (void**)&original_no_front);
    void* side_off = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.RunnerCore.Character", "CharacterMotor", "CheckSideImpact", 1);
    DobbyHook(side_off, (void*)no_side, (void**)&original_no_side);
    void* camera_off = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway", "CameraGroundedModifier", "Apply", 1);
    DobbyHook(camera_off, (void*)follow_camera, (void**)&original_camera);
    void* stopTrain = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway", "MovingTrain", "Disable", 0);
    DobbyHook(stopTrain, (void*)train, (void**)&old_train);
    void* minSpeedOff = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway", "DefaultSpeedController", "get_MinSpeed", 0);
    DobbyHook(minSpeedOff, (void*)getMinSpeed, (void**)&original_speed);
    void* minSpeedOff1 = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway", "DefaultSpeedController", "get_MaxSpeed", 0);
    DobbyHook(minSpeedOff1, (void*)getMinSpeed1, (void**)&original_speed1);
}
void touch(bool* mouse) {
    ImGuiIO& io = ImGui::GetIO();
    int (*TouchCount)(void*) = (int (*)(void*)) (Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Input", "get_touchCount", 0));
    int touchCount = TouchCount(nullptr);
    if (touchCount > 0) {
        UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields (*)(int)) (Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Input", "GetTouch", 1))) (0);
        float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;

        switch (touch.m_Phase) {
            case TouchPhase::Began:
            case TouchPhase::Stationary:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                io.MouseDown[0] = true;
                break;
            case TouchPhase::Ended:
            case TouchPhase::Canceled:
                io.MouseDown[0] = false;
                *mouse = true;
                break;
            case TouchPhase::Moved:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                break;
            default:
                break;
        }
    } else {
        io.MouseDown[0] = false;
    }
}

EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffer(EGLDisplay dpy, EGLSurface surface) {
    static bool g_Initialized = false;
    static bool should_clear_mouse_pos = false;
    if (!g_Initialized) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        ImGui_ImplOpenGL3_Init("#version 300 es");
        ImGui::StyleColorsDark();
        g_Initialized = true;
    }

    static bool idk = false;
    static float value = 0.0f;
    EGLint w, h;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &w);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &h);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    touch(&should_clear_mouse_pos);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Dear ImGui");
    ImGui::Text("Android!");
    ImGui::Checkbox("Click", &idk);
    ImGui::SliderFloat("Value",&value,0.0f,100.0f);
    ImGui::Text("Bye!");
    ImGui::Checkbox("Shop", &shop);
    ImGui::Checkbox("Jump", &jump);
    ImGui::Checkbox("No Death", &no_dealth);
    ImGui::Checkbox("Stop Train", &stopTrain);
    ImGui::SliderFloat("Speed", &speed, 0.0f, 500.0f);
    ImGui::SliderFloat("Speed1", &speed1, 0.0f, 500.0f);
    ImGui::Checkbox("Debug", &debug);
    ImGui::End();

    if (debug) {
        ImGui::Begin("Debug");
        ImGui::Text("Debug Menu");
        ImGui::Text("FPS  %.1f", ImGui::GetIO().Framerate);
        ImGui::Checkbox("Patch Memory", &patch);
        ImGui::Checkbox("Patch Shop", &patch1);
        void* shop = (void*)(base + 0x3CB5574);
	//void* shop = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway.Core.CommonData", "Currency", "get_IsIAP", 0);
        void* jump_off = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.RunnerCore.Character", "CharacterMotor", "get_CanJump", 0);
        void* minSpeedOffset = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway", "DefaultSpeedController", "get_MinSpeed", 0);
    void* minSpeedOffset1 = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SYBO.Subway", "DefaultSpeedController", "get_MaxSpeed", 0);
        ImGui::Text("Lib base address: %p", (void*)base);
        ImGui::Text("Shop Offset: %p", shop);
        ImGui::Text("Jump Offset: %p", jump_off);
        ImGui::Text("Min speed: %p", minSpeedOffset);
        ImGui::Text("Max speed: %p", minSpeedOffset1);
        if (patch) {
            unsigned char bytes[] = {
                0x20, 0x00, 0x80, 0x52,  // mov w0, #1
                0xc0, 0x03, 0x5f, 0xd6   // ret
            };
            patchMemory((uintptr_t)jump_off, bytes, sizeof(bytes));
        }
        if (patch1) {
            unsigned char bytes[] = {
                0x00, 0x00, 0x80, 0x52,  // mov w0, #1
                0xc0, 0x03, 0x5f, 0xd6   // ret
            };
            patchMemory((uintptr_t)shop, bytes, sizeof(bytes));
        }

        ImGui::End();
    }
    ImGui::Render();
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (should_clear_mouse_pos) {
        io.MousePos = ImVec2(-1, -1);
        should_clear_mouse_pos = false;
    }
    return orig_eglSwapBuffers(dpy, surface);

}
void *sylphy(void*) {
    
    while ((base = GetBaseAdress("libil2cpp.so")) == 0) {
    sleep(3);
    }


    Il2CppAttach("libil2cpp.so");
    void *egl = dlopen("libEGL.so", RTLD_NOW);
    if (!egl) {
        return nullptr;
    }
    void *swap = dlsym(egl, "eglSwapBuffers");
    if (!swap) {
        return nullptr;
    }
    DobbyHook(swap, (void*)hook_eglSwapBuffer, (void**)&orig_eglSwapBuffers); 
    hack();
    return nullptr;
}
__attribute__((constructor))
void lib_main() {
    pthread_t trixie;
    pthread_create(&trixie, NULL, sylphy, NULL);
    
}
