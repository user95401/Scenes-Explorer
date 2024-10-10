#include <_main.hpp>
#include <_imgui.hpp>

auto IniFile = fs::path(getMod()->getSaveDir() / "imgui.ini");

auto Setup() {
	ImGui::GetIO().IniFilename = IniFile.string().c_str();

	auto& io = ImGui::GetIO();
	io.FontDefault = io.Fonts->AddFontFromFileTTF(
		CCFileUtils::get()->fullPathForFilename("consolas.ttf", 0).c_str(),
		24
	);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavNoCaptureKeyboard;
	io.NavActive = true;
	io.NavVisible = true;
	io.FontAllowUserScaling = true;
	io.BackendFlags = true;

	static auto IniFileStr = std::string(IniFile.string());
	io.IniFilename = IniFileStr.c_str();

	auto& style = ImGui::GetStyle();
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowBorderSize = 2.5;
	style.ColorButtonPosition = ImGuiDir_Left;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.34f);

#ifdef GEODE_IS_MOBILE
	ImGui::GetStyle().ScrollbarSize = 42.f;
	//ImGui::GetStyle().TabBarBorderSize = 60.f;
	ImGui::GetStyle().GrabMinSize = 30.f;
	ImGui::GetStyle().ItemSpacing = { 16.f, 16.f };
	ImGui::GetStyle().FramePadding = { 12.f, 10.f };
	io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
#endif

}

static CCNode* HoveredNode = nullptr;
static CCNode* SelectedNode = nullptr;
static bool ReachedSelectedNode = false;

namespace Tabs {
	auto Tree = true;
	auto Scenes = false;
	auto Settings = false;
	auto DearImGuiWindows = false;
};

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)

void RenderNodeTree(CCNode* node, unsigned int index = 0) {
	if (!node) return;
	std::stringstream stream;
	stream << "[" << index << "] " << getNodeName(node);
	if (node->getTag() != -1)
		stream << " (" << node->getTag() << ")";
	const auto children_count = node->getChildrenCount();
	if (children_count)
		stream << " {" << children_count << "}";

	auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
	if (SelectedNode == node) {
		flags |= ImGuiTreeNodeFlags_Selected;
		ReachedSelectedNode = true;
	}
	if (node->getChildrenCount() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	ImGui::PushStyleColor(ImGuiCol_Text, node->isVisible() ? ImVec4{ 1.f, 1.f, 1.f, 1.f } : ImVec4{ 0.8f, 0.8f, 0.8f, 1.f });
	const bool is_open = ImGui::TreeNodeEx(node, flags, "%s", stream.str().data());
	if (ImGui::IsItemClicked()) {
		if (node == SelectedNode) {
			SelectedNode = nullptr;
			ReachedSelectedNode = false;
		}
		else {
			//SaveOldPropertiesStore(node);
			SelectedNode = node;
			ReachedSelectedNode = true;
		}
	}
	if (ImGui::IsItemHovered())
		HoveredNode = node;
	if (is_open) {
		auto children = node->getChildren();
		for (unsigned int i = 0; i < children_count; ++i) {
			auto child = children->objectAtIndex(i);
			RenderNodeTree(static_cast<CCNode*>(child), i);
		}
		ImGui::TreePop();
	}
	ImGui::PopStyleColor();
}

void RenderNodeProperties(CCNode* node) {
	if (!node) return;
	if (ImGui::Button("Delete")) {
		node->removeFromParentAndCleanup(true);
		return;
	}
	ImGui::SameLine();
	bool R = (GetAsyncKeyState(0x52) & 0x8000);
	if (ImGui::Button("Reset All") or R) {
		//node->setAnchorPoint(OldAnchorPoint);
		//node->setPosition(OldPosition);
		//node->setRotation(OldRotation);
		//node->setScaleX(OldScaleX);
		//node->setScaleY(OldScaleY);
		//node->setContentSize(OldContentSize);
		//node->setVisible(OldVisible);
	}
	ImGui::Text("Addr: 0x%p", node);
	ImGui::SameLine();
	if (ImGui::Button("Copy##copyaddr")) {
		std::stringstream stream;
		stream << std::uppercase << std::hex << reinterpret_cast<uintptr_t>(node);
		//set_clipboard_text(stream.str());
	}
	if (node->getUserData())
		ImGui::Text("User data: 0x%p", node->getUserData());

#define GET_SET_FLOAT2(name, label) { \
	auto point = node->get##name(); \
	if (ImGui::DragFloat2(label, reinterpret_cast<float*>(&point))) \
		node->set##name(point); \
}

#define GET_SET_INT(name, label) { \
	auto value = node->get##name(); \
	if (ImGui::InputInt(label, &value)) \
		node->set##name(value); \
}

#define GET_SET_CHECKBOX(name, label) { \
	auto value = node->is##name(); \
	if (ImGui::Checkbox(label, &value)) \
		node->set##name(value); \
}

	GET_SET_FLOAT2(Position, "Position");

#define dragXYBothIDK(name, label, speed) { \
	float values[3] = { node->get##name(), node->CONCAT(get, CONCAT(name, X))(), node->CONCAT(get, CONCAT(name, Y))() }; \
	if (ImGui::DragFloat3(label, values, speed)) { \
		if (node->get##name() != values[0]) \
			node->set##name(values[0]); \
		else { \
			node->CONCAT(set, CONCAT(name, X))(values[1]); \
			node->CONCAT(set, CONCAT(name, Y))(values[2]); \
		} \
	} \
}
	dragXYBothIDK(Scale, "Scale", 0.025f);
	dragXYBothIDK(Rotation, "Rotation", 1.0f);

#undef dragXYBothIDK

	{
		auto anchor = node->getAnchorPoint();
		if (ImGui::DragFloat2("Anchor Point", &anchor.x, 0.05f, 0.f, 1.f))
			node->setAnchorPoint(anchor);
	}

	GET_SET_FLOAT2(ContentSize, "Content Size");
	GET_SET_INT(ZOrder, "Z Order");
	GET_SET_CHECKBOX(Visible, "Visible");

	if (auto rgba_node = dynamic_cast<CCNodeRGBA*>(node); rgba_node) {
		auto color = rgba_node->getColor();
		float colorValues[4] = {
			color.r / 255.f,
			color.g / 255.f,
			color.b / 255.f,
			rgba_node->getOpacity() / 255.f
		};
		if (ImGui::ColorEdit4("Color", colorValues)) {
			rgba_node->setColor({
				static_cast<GLubyte>(colorValues[0] * 255),
				static_cast<GLubyte>(colorValues[1] * 255),
				static_cast<GLubyte>(colorValues[2] * 255)
				});
			rgba_node->setOpacity(static_cast<GLubyte>(colorValues[3] * 255.f));
		}
	}

	if (auto label_node = dynamic_cast<CCLabelProtocol*>(node); label_node) {
		std::string str = label_node->getString();
		if (ImGui::InputTextMultiline("Text", str.data(), str.size() + 1, {0, 50}))
			label_node->setString(str.c_str());
	}

	if (auto sprite_node = dynamic_cast<CCSprite*>(node); sprite_node) {
		auto* texture = sprite_node->getTexture();

		auto* texture_cache = CCTextureCache::sharedTextureCache();
		auto* cached_textures = public_cast(texture_cache, m_pTextures);
		CCDictElement* el;
		CCDICT_FOREACH(cached_textures, el) {
			if (el->getObject() == texture) {
				ImGui::TextWrapped("Texture name: %s", el->getStrKey());
				break;
			}
		}

		auto* frame_cache = CCSpriteFrameCache::sharedSpriteFrameCache();
		auto* cached_frames = public_cast(frame_cache, m_pSpriteFrames);
		const auto rect = sprite_node->getTextureRect();
		CCDICT_FOREACH(cached_frames, el) {
			auto* frame = static_cast<CCSpriteFrame*>(el->getObject());
			if (frame->getTexture() == texture && frame->getRect().equals(rect)) {
				ImGui::Text("Frame name: %s", el->getStrKey());
				break;
			}
		}
	}

	if (auto menu_item_node = dynamic_cast<CCMenuItem*>(node); menu_item_node) {
		const auto selector = public_cast(menu_item_node, m_pfnSelector);
		const auto addr = std::string("TODO");//format_addr(union_cast<void*>(selector));
		ImGui::Text("CCMenuItem selector: %s", addr.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Copy##copyselector")) {
			//set_clipboard_text(addr);
		}
	}
}

auto Draw() {

	auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	//update by settings
	io.FontGlobalScale = getMod()->getSavedValue("ImGui.FontGlobalScale", io.FontGlobalScale);


	if (ImGui::Begin("Scenes Explorer", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar)) {

		ImGui::BeginMenuBar(); {
			if (ImGui::MenuItem("Tree", "1", &Tabs::Tree)) {
				Tabs::Tree = 1;
				Tabs::Scenes = 0;
				Tabs::Settings = 0;
			};
			if (ImGui::MenuItem("Scenes", "2", &Tabs::Scenes)) {
				Tabs::Tree = 0;
				Tabs::Scenes = 1;
				Tabs::Settings = 0;
			};
			if (ImGui::MenuItem("Settings", "3", &Tabs::Settings)) {
				Tabs::Tree = 0;
				Tabs::Scenes = 0;
				Tabs::Settings = 1;
			};
		};
		ImGui::EndMenuBar();

		if (Tabs::Tree) {
			ImGui::BeginChild("explorer.tree", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0), ImGuiChildFlags_ResizeX, ImGuiWindowFlags_HorizontalScrollbar); {

				CCNode* node = CCScene::get();
				if (node) {
					ReachedSelectedNode = false;
					RenderNodeTree(node);
				}
			};
            ImGui::EndChild();
			if (!ReachedSelectedNode) SelectedNode = nullptr;
			ImGui::SameLine();
			ImGui::BeginChild("explorer.tree.selected_node"); {
				if (SelectedNode) {
					RenderNodeProperties(SelectedNode);
				}
				else ImGui::Text("Select a node to edit its properties");
			};
			ImGui::EndChild();
		}

		if (Tabs::Scenes) {
			ImGui::Text("the fucking Scenes here");
			ImGui::BeginChild("explorer.scenes");

			ImGui::EndChild();
		}

		if (Tabs::Settings) {
			ImGui::Text("the fucking Settings here");

			ImGui::BeginChild("explorer.settings"); {
				if (ImGui::DragFloat("Font Global Scale", &io.FontGlobalScale, 0.01, 0.1, ImGuiInputTextFlags_ReadOnly)) {
					getMod()->setSavedValue("ImGui.FontGlobalScale", io.FontGlobalScale);
				}
			};
			ImGui::EndChild();
		}

	} ImGui::End();

	ImGui::Begin("Dear ImGui Style Editor"); {
		ImGui::ShowStyleEditor();
	}
	ImGui::End();

	ImGui::ShowMetricsWindow();
}

inline auto ModLoaded() {
    auto ew = &ImGuiCocos::get();
    ew->setup([]() { Setup(); });
    ew->draw([]() { Draw(); });
    ew->setVisible(1);//SETTING(bool, "Visible"));
}

$on_mod(Loaded) {
	ModLoaded();
}