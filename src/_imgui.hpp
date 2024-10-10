#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <imgui-cocos.hpp>

bool operator!=(const cocos2d::CCSize& a, const cocos2d::CCSize& b) { return a.width != b.width || a.height != b.height; }
ImVec2 operator*(const ImVec2& vec, const float m) { return { vec.x * m, vec.y * m }; }
ImVec2 operator/(const ImVec2& vec, const float m) { return { vec.x / m, vec.y / m }; }
ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return { a.x + b.x, a.y + b.y }; }
ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return { a.x - b.x, a.y - b.y }; }

bool operator==(const cocos2d::CCPoint& a, const cocos2d::CCPoint& b) { return a.x == b.x && a.y == b.y; }
bool operator==(const cocos2d::CCRect& a, const cocos2d::CCRect& b) { return a.origin.equals(b.origin) && a.size == b.size; }

cocos2d::CCPoint& operator-=(cocos2d::CCPoint& point, const cocos2d::CCPoint& other) {
	point = point - other;
	return point;
}

ImVec2 cocos_to_vec2(const cocos2d::CCPoint& a) {
	const auto size = ImGui::GetMainViewport()->Size;
	const auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
	return {
		a.x / win_size.width * size.x,
		(1.f - a.y / win_size.height) * size.y
	};
}

ImVec2 cocos_to_vec2(const cocos2d::CCSize& a) {
	const auto size = ImGui::GetMainViewport()->Size;
	const auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
	return {
		a.width / win_size.width * size.x,
		-a.height / win_size.height * size.y
	};
}

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