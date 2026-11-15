#include <Geode/Geode.hpp>

#include <imgui-cocos.hpp>

#include "common.hh"

namespace uv {
    std::vector<std::string> installed_conflict_mods;
};

$on_mod(Loaded) {
    ImGuiCocos::get().setup(uv::gui::setup).draw(uv::gui::draw);
    uv::practice_fix::log_missing_fields();
    
    std::vector<std::filesystem::path> paths = {
        uv::macro_path,
        uv::showcase_path,
    };

    for (auto &path : paths) {
        std::error_code error;
        if (std::filesystem::is_directory(path, error)) continue;
        std::filesystem::create_directories(path, error);
        if (error) geode::log::warn("Couldn't create '{}': {}", uv::path_str(path), error.message());
    }
    
    const std::vector<std::string> conflict_mods = {
        "syzzi.click_between_frames",
        "tobyadd.gdh",
    };

    for (auto &mod : conflict_mods) {
        geode::Mod *modobj = geode::Loader::get()->getLoadedMod(mod);
        if (modobj) uv::installed_conflict_mods.push_back(modobj->getName());
    }

    if (!uv::installed_conflict_mods.empty()) {
        geode::Mod *uv = geode::Mod::get();
        std::vector<geode::Hook*> hooks = uv->getHooks();
        const std::unordered_set<std::string> essential_hooks = {
            "cocos2d::CCMouseDispatcher::dispatchScrollMSG",
            "cocos2d::CCIMEDispatcher::dispatchDeleteBackward",
            "cocos2d::CCIMEDispatcher::dispatchInsertText",
            "cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG",
            "cocos2d::CCTouchDispatcher::touches",
            "cocos2d::CCEGLView::swapBuffers",
            "cocos2d::CCEGLView::toggleFullScreen",
            "cocos2d::CCEGLView::onGLFWKeyCallback",
        };
        for (auto &i : hooks) {
            if (essential_hooks.contains(static_cast<std::string>(i->getDisplayName()))) continue;
            if (!i->disable().isOk()) geode::log::warn("Couldn't panic disable hook {}", i->getDisplayName());
        }
    }
}
