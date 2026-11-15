#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/CheckpointObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "common.hh"

namespace uv::practice_fix {
    namespace {
        template <class P, class D>
        void save_fields(P *po, D &data) {
            #define UV_CP_SAVE(name) if constexpr (requires { po->name; }) data.name = po->name;
            UV_CP_FIELDS(UV_CP_SAVE)
            #undef UV_CP_SAVE
        }

        template <class P, class D>
        void load_fields(P *po, D const &data) {
            #define UV_CP_LOAD(name) if constexpr (requires { po->name; }) po->name = data.name;
            UV_CP_FIELDS(UV_CP_LOAD)
            #undef UV_CP_LOAD
        }

        template <class P>
        std::vector<std::string> missing_fields() {
            std::vector<std::string> missing;
            #define UV_CP_MISSING(name) if constexpr (!requires(P *p) { p->name; }) missing.push_back(#name);
            UV_CP_FIELDS(UV_CP_MISSING)
            #undef UV_CP_MISSING
            return missing;
        }
    }

    void log_missing_fields(void) {
        std::vector<std::string> missing = missing_fields<PlayerObject>();
        if (missing.empty()) return;
        geode::log::warn("Practice fix skips {} PlayerObject fields that do not exist in this GD version: {}", missing.size(), fmt::join(missing, ", "));
    }

    checkpoint_data from_playerobject(PlayerObject *po) {
        checkpoint_data data;
        data.rotation = po->getRotation();
        save_fields(po, data);
        return data;
    }

    void restore_playerobject(PlayerObject *po, checkpoint_data data) {
        po->setRotation(data.rotation);
        load_fields(po, data);
    }
}

static bool override_rotation = false;

// Funky GD physics lmao
// Thanks ChatGPT

class $modify(PlayerObject) {
    void updateRotation(float p0) {
        float old_rotation = this->getRotation();
        
        PlayerObject::updateRotation(p0);
        
        if (override_rotation) {
            override_rotation = false;
            this->setRotation(old_rotation);
        }
    }
};

class $modify(HookedCheckpointObject, CheckpointObject) {
    struct Fields {
        uv::practice_fix::checkpoint_data p1, p2;
    };

    bool init(void) {
        //  | This one byte of code just cost me an hour of painful debugging
        //  v I will never get that time back
        if (!CheckpointObject::init()) return false;
        
        PlayLayer *pl = PlayLayer::get();

        if (pl) {
            m_fields->p1 = uv::practice_fix::from_playerobject(pl->m_player1);
            if (pl->m_player2) m_fields->p2 = uv::practice_fix::from_playerobject(pl->m_player2);
        }
        
        return true;
    }
};

class $modify(PlayLayer) {
    void loadFromCheckpoint(CheckpointObject *cp) {
        PlayLayer::loadFromCheckpoint(cp);

        if (uv::hacks::get<bool>("practice-fix", true)) {
            uv::practice_fix::restore_playerobject(this->m_player1, static_cast<HookedCheckpointObject*>(cp)->m_fields->p1);
            if (this->m_player2) uv::practice_fix::restore_playerobject(this->m_player2, static_cast<HookedCheckpointObject*>(cp)->m_fields->p2);
            override_rotation = true;
        }
    }
};
