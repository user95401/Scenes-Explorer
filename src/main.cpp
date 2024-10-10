#include <_main.hpp>
#include <_imgui.hpp>

Ref<CCNodeRGBA> ImGuiHelperNode;

auto Setup() {
	
	ImGuiHelperNode = CCNodeRGBA::create();
	ImGuiHelperNode->setOpacity(ImGui::GetStyle().Alpha * 255);

	auto& io = ImGui::GetIO();
	auto FontFile = getMod()->getResourcesDir() / "consolas.ttf";
	if (fs::exists(FontFile)) io.FontDefault = io.Fonts->AddFontFromFileTTF(FontFile.string().c_str(), 24);
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
	io.NavActive = true;
	io.NavVisible = true;
	io.FontAllowUserScaling = true;

	auto& style = ImGui::GetStyle();
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowBorderSize = 2.5;
	style.ColorButtonPosition = ImGuiDir_Left;
	style.FramePadding = { 6.f, 6.f };

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.34f);

#ifdef GEODE_IS_MOBILE
	ImGui::GetStyle().ScrollbarSize = 42.f;
	//ImGui::GetStyle().TabBarBorderSize = 60.f;
	ImGui::GetStyle().GrabMinSize = 30.f;
	ImGui::GetStyle().ItemSpacing = { 16.f, 16.f };
	ImGui::GetStyle().FramePadding = { 12.f, 10.f };
	//io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
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
	if (node->getID().size() > 0) stream << " \"" << node->getID() << "\"";
	if (node->getTag() != -1) stream << " (" << node->getTag() << ")";
	if (node->getChildrenCount()) stream << " {" << node->getChildrenCount() << "}";

	auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
	
	if (SelectedNode == node) {
		flags |= ImGuiTreeNodeFlags_Selected;
		ReachedSelectedNode = true;
	}
	if (node->getChildrenCount() == 0) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;

	auto TextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
	auto TextColorDark = TextColor; TextColorDark.w = 0.8;
	auto TextColorDarker = TextColor; TextColorDarker.w = 0.6;
	auto TextColorToPush = TextColor;
	TextColorToPush = cocos::nodeIsVisible(node) ? TextColorToPush : TextColorDark;
	TextColorToPush = node->isVisible() ? TextColorToPush : TextColorDarker;
	ImGui::PushStyleColor(ImGuiCol_Text, TextColorToPush);

	const bool is_open = ImGui::TreeNodeEx(node, flags, "%s", stream.str().data());

	HoveredNode = ImGui::IsItemHovered() ? node : HoveredNode;

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

	if (is_open) {
		for (unsigned int i = 0; i < node->getChildrenCount(); ++i) {
			auto child = node->getChildren()->objectAtIndex(i);
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
	bool R = (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_R));
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

//DevTools
enum class HighlightMode {
	Selected,
	Hovered,
	Layout,
};
void DrawHighlight(CCNode* node, HighlightMode mode) {
	auto& background = *ImGui::GetBackgroundDrawList();
	auto parent = node->getParent();
	auto bounding_box = node->boundingBox();
	CCPoint bb_min(bounding_box.getMinX(), bounding_box.getMinY());
	CCPoint bb_max(bounding_box.getMaxX(), bounding_box.getMaxY());

#ifdef GEODE_IS_WINDOWS
	auto cameraParent = node;
	while (cameraParent) {
		auto camera = cameraParent->getCamera();

		float off_x, off_y, off_z;
		camera->getEyeXYZ(&off_x, &off_y, &off_z);
		const CCPoint offset(off_x, off_y);
		bb_min -= offset;
		bb_max -= offset;

		cameraParent = cameraParent->getParent();
	}
#endif

	auto min = toVec2(parent ? parent->convertToWorldSpace(bb_min) : bb_min);
	auto max = toVec2(parent ? parent->convertToWorldSpace(bb_max) : bb_max);

	auto wsize = ImGui::GetMainViewport()->Size;

	auto anchor = ImVec2(
		node->getAnchorPoint().x * (max.x - min.x) + min.x,
		node->getAnchorPoint().y * (max.y - min.y) + min.y
	);

	switch (mode) {
	case HighlightMode::Selected: {
		background.AddRect(
			min, max, IM_COL32(0, 255, 55, 155),
			0.f, 0, 3.f
		);
		background.AddCircleFilled(
			anchor, 7.5f, IM_COL32(255, 75, 105, 255)
		);
		background.AddCircleFilled(
			anchor, 5.f, IM_COL32(255, 255, 255, 255)
		);
		if (auto layout = typeinfo_cast<AxisLayout*>(node->getLayout())) {
			//drawLayoutArrows(background, layout, tmax, tmin);
		}
	} break;

	case HighlightMode::Layout: {
		background.AddRect(
			min, max, IM_COL32(255, 155, 55, 255),
			0.f, 0, 4.f
		);
		// built-in Geode layouts get special extra markings
		if (auto layout = typeinfo_cast<AxisLayout*>(node->getLayout())) {
			//drawLayoutArrows(background, layout, tmax, tmin);
		}
	} break;

	default:
	case HighlightMode::Hovered: {
		background.AddRectFilled(
			min, max, IM_COL32(0, 255, 55, 70)
		);
	} break;

	}
}

auto Draw() {

	auto& io = ImGui::GetIO();
	auto& style = ImGui::GetStyle();

	if (io.NavVisible) io.WantCaptureKeyboard = true;

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

				CCNode* node = CCDirector::get()->m_pRunningScene;
				if (node) {
					ReachedSelectedNode = false;
					RenderNodeTree(node);
					RenderNodeTree(ImGuiHelperNode);
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

	} 
	ImGui::End();

	if (HoveredNode and HoveredNode->isRunning()) DrawHighlight(HoveredNode, HighlightMode::Hovered);
	if (SelectedNode and SelectedNode->isRunning()) DrawHighlight(SelectedNode, HighlightMode::Selected);

	ImGui::Begin("Dear ImGui Style Editor"); {
		ImGui::ShowStyleEditor();
	}
	ImGui::End();

	ImGui::ShowMetricsWindow();

	//update by settings
	io.FontGlobalScale = getMod()->getSavedValue("ImGui.FontGlobalScale", io.FontGlobalScale);

	ImGui::GetStyle().Alpha = ImGuiHelperNode->getOpacity() / 255;
}

#include <Geode/modify/CCScene.hpp>
class $modify(ScenesExplorerLoader, CCScene) {
	inline static auto pImGuiCocos = &ImGuiCocos::get();
	inline static auto Loaded = false;
	inline static auto TryLoad() {
		if (Loaded) return; else Loaded = true;
		pImGuiCocos->setup([]() { Setup(); });
		pImGuiCocos->draw([]() { Draw(); });
		pImGuiCocos->setVisible(1);//SETTING(bool, "Visible"));
	}
	$override static CCScene* create(void) {
		auto __this = CCScene::create();
		TryLoad();
		//scroll
		auto size = CCDirector::get()->getVisibleSize();
		size.width = 120.f;
		auto scroll = CCScrollView::create(size);
		scroll->runAction(CCRepeatForever::create(CCSpawn::create(CCLambdaAction::create(
			[scroll]() {
				if (scroll->getContainer()->getPositionX() > 50.f) {
					pImGuiCocos->setVisible(1);
					scroll->getContainer()->setPositionX(0.f);
					scroll->setTouchEnabled(0);
					scroll->setTouchEnabled(1);
				}
				if (scroll->getContainer()->getPositionX() < -50.f) {
					pImGuiCocos->setVisible(0);
					scroll->getContainer()->setPositionX(0.f);
					scroll->setTouchEnabled(0);
					scroll->setTouchEnabled(1);
				}
			}
		), nullptr)));
		__this->addChild(scroll);
		return __this;
	}
};