#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <imgui-cocos.hpp>

static std::ostream& operator<<(std::ostream& stream, ImVec2 const& vec) {
    return stream << vec.x << ", " << vec.y;
}

static std::ostream& operator<<(std::ostream& stream, ImVec4 const& vec) {
    return stream << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
}

static ImVec2 operator+(ImVec2 const& a, ImVec2 const& b) {
    return {
        a.x + b.x,
        a.y + b.y
    };
}

static ImVec2 operator-(ImVec2 const& a, ImVec2 const& b) {
    return {
        a.x - b.x,
        a.y - b.y
    };
}

static ImVec2 operator-(ImVec2 const& a) {
    return { -a.x, -a.y };
}

static ImVec2& operator-=(ImVec2& a, ImVec2 const& b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

static ImVec2 operator/(ImVec2 const& a, ImVec2 const& b) {
    return {
        a.x / b.x,
        a.y / b.y
    };
}

static ImVec2 operator/(ImVec2 const& a, int b) {
    return {
        a.x / b,
        a.y / b
    };
}

static ImVec2 operator/(ImVec2 const& a, float b) {
    return {
        a.x / b,
        a.y / b
    };
}

static bool operator!=(ImVec2 const& a, ImVec2 const& b) {
    return a.x != b.x || a.y != b.y;
}

static ImVec2 operator*(ImVec2 const& v1, float multi) {
    return { v1.x * multi, v1.y * multi };
}

static ImVec2 toVec2(CCPoint const& a) {
    const auto size = ImGui::GetMainViewport()->Size;
    const auto winSize = CCDirector::get()->getWinSize();
    return {
        a.x / winSize.width * size.x,
        (1.f - a.y / winSize.height) * size.y
    };
}

static ImVec2 toVec2(CCSize const& a) {
    const auto size = ImGui::GetMainViewport()->Size;
    const auto winSize = CCDirector::get()->getWinSize();
    return {
        a.width / winSize.width * size.x,
        -a.height / winSize.height * size.y
    };
}

static CCPoint toCocos(const ImVec2& pos) {
    const auto winSize = CCDirector::sharedDirector()->getWinSize();
    const auto size = ImGui::GetMainViewport()->Size;
    return CCPoint(
        pos.x / size.x * winSize.width,
        (1.f - pos.y / size.y) * winSize.height
    );
};

namespace ImGui {
    inline auto AddTooltip(const char* text) {
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::SetTooltip("%s", text);
        }
        return;
    }
}

#define AddKeyEvent(keyName) if (key == KEY_##keyName) io.AddKeyAnalogEvent(ImGuiKey_##keyName, down, 1.f)
#define AddKeyModEvent(keyName) if (key == KEY_##keyName) io.AddKeyAnalogEvent(ImGuiKey_Mod##keyName, down, 1.f);
#define AddKeyEventArrow(keyName) if (key == KEY_##keyName or key == KEY_Arrow##keyName) io.AddKeyAnalogEvent(ImGuiKey_##keyName##Arrow, down, 1.f)
#include <Geode/modify/CCKeyboardDispatcher.hpp>
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool arr) {
        //log::debug("{}({},{},{})", __FUNCTION__, CCKeyboardDispatcher::get()->keyToString(((int)key > 1 ? key : KEY_ApplicationsKey)), down, arr);
        auto& io = ImGui::GetIO();
        {
            if (key == KEY_Control) io.AddKeyAnalogEvent(ImGuiKey_ModCtrl, down, 1.f);
            AddKeyModEvent(Shift);
            AddKeyModEvent(Alt);
            //fucking sucks
            AddKeyEventArrow(Left);
            AddKeyEventArrow(Right);
            AddKeyEventArrow(Down);
            AddKeyEventArrow(Up);
            AddKeyEvent(Space);
            AddKeyEvent(Backspace);//0x09,
            AddKeyEvent(Tab);//0x09,
            AddKeyEvent(Enter);//0x0D,
            AddKeyEvent(Pause);//0x13,
            AddKeyEvent(CapsLock);//0x14,
            AddKeyEvent(Escape);//0x1B,
            AddKeyEvent(Space);//0x20,
            AddKeyEvent(PageUp);//0x21,
            AddKeyEvent(PageDown);//0x22,
            AddKeyEvent(End);//0x23,
            AddKeyEvent(Home);//0x24,
            AddKeyEvent(PrintScreen);//0x2C,
            AddKeyEvent(Insert);//0x2D,
            AddKeyEvent(Delete);//0x2E,
            AddKeyEvent(A);//0x41,
            AddKeyEvent(B);//0x42,
            AddKeyEvent(C);//0x43,
            AddKeyEvent(D);//0x44,
            AddKeyEvent(E);//0x45,
            AddKeyEvent(F);//0x46,
            AddKeyEvent(G);//0x47,
            AddKeyEvent(H);//0x48,
            AddKeyEvent(I);//0x49,
            AddKeyEvent(J);//0x4A,
            AddKeyEvent(K);//0x4B,
            AddKeyEvent(L);//0x4C,
            AddKeyEvent(M);//0x4D,
            AddKeyEvent(N);//0x4E,
            AddKeyEvent(O);//0x4F,
            AddKeyEvent(P);//0x50,
            AddKeyEvent(Q);//0x51,
            AddKeyEvent(R);//0x52,
            AddKeyEvent(S);//0x53,
            AddKeyEvent(T);//0x54,
            AddKeyEvent(U);//0x55,
            AddKeyEvent(V);//0x56,
            AddKeyEvent(W);//0x57,
            AddKeyEvent(X);//0x58,
            AddKeyEvent(Y);//0x59,
            AddKeyEvent(Z);//0x5A,
            AddKeyEvent(F1);//0x70,
            AddKeyEvent(F2);//0x71,
            AddKeyEvent(F3);//0x72,
            AddKeyEvent(F4);//0x73,
            AddKeyEvent(F5);//0x74,
            AddKeyEvent(F6);//0x75,
            AddKeyEvent(F7);//0x76,
            AddKeyEvent(F8);//0x77,
            AddKeyEvent(F9);//0x78,
            AddKeyEvent(F10);//0x79,
            AddKeyEvent(F11);//0x7A,
            AddKeyEvent(F12);//0x7B,
            AddKeyEvent(ScrollLock);//0x91,
        };
        if (io.WantCaptureKeyboard) return true;
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, arr);
    }
};