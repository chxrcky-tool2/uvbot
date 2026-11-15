#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Geode/Geode.hpp>

#include <asp/fs/std.hpp>

namespace uv {
    inline std::filesystem::path std_path(std::filesystem::path p) {
        return p;
    }

    inline std::filesystem::path std_path(asp::fs::Path const &p) {
        return p.toStdPath().unwrapOr(std::filesystem::path{});
    }

    inline std::string path_str(std::filesystem::path const &p) {
        auto s = p.u8string();
        return std::string(reinterpret_cast<const char*>(s.data()), s.size());
    }

    inline void open_folder(std::filesystem::path const &p) {
        geode::utils::file::openFolder(asp::fs::Path::fromStdPath(p));
    }

    const std::filesystem::path macro_path = std_path(geode::Mod::get()->getSaveDir()) / "Macros";
    const std::filesystem::path showcase_path = std_path(geode::Mod::get()->getSaveDir()) / "Showcases";
    extern std::vector<std::string> installed_conflict_mods;
    
    namespace gui {
        extern bool show;
        extern bool debug;
        extern std::chrono::time_point<std::chrono::steady_clock> toggle_time;
        
        void setup(void);
        void draw(void);
    }

    namespace bot {
        // Frame type (u64)
        typedef unsigned long long frame_t;
        
        struct input_action {
            frame_t frame;
            unsigned char flags;
        };

        struct physic_action {
            frame_t frame;
            float x, y, rotation;
            double y_velocity;
        };

        extern std::vector<input_action> input_actions;
        extern std::vector<physic_action> physic_player_1_actions;
        extern std::vector<physic_action> physic_player_2_actions;

        extern int current_input_action;
        extern int current_physic_player_1_action;
        extern int current_physic_player_2_action;
        
        enum state: int {
            none = 0,
            recording,
            playing,
        };
        
        extern state current_state;
        
        extern float record_tps;
        
        frame_t get_frame(void);
        
        void reset(void);
        bool button(bool held, int button, bool player_1);
        void update_input(GJBaseGameLayer *self, float delta);
        void update_physics(GJBaseGameLayer *self, float delta);

        void clear(void);
        void save(std::string name);
        void load(std::string name);

        namespace foreign {
            enum error {
                none = 0,
                unsupported,
                unsupported_slc_v2,
                unsupported_slc_v3,
            };
            
            extern std::vector<std::string> supported_exts;

            error load(std::string name);
        }
    }
    
    namespace recorder {
        struct options {
            int width, height;
            float fps, excess_render;
            std::string codec, bitrate;
            std::string output_path;
            std::string custom_options;
            bool hide_end_level_screen, fade_out;
        };

        void start(options opt);
        void update(void);
        void end(void);

        extern bool recording;
        extern options recording_options;
        
        namespace audio {
            struct options {
                std::string output_path;
                float music_volume, sfx_volume;
                float excess_render;
            };
            
            void init(options opt);
            void start(void);
            void end(void);

            extern bool recording;
            extern options recording_options;
        }

        void merge(std::string video, std::string audio, std::string output, std::string args);
        void raw(std::string args);
        bool process_done(void);
    }
    
    namespace hacks {
        template <typename T = bool>
        T get(std::string id, T d) {
            return geode::Mod::get()->getSavedValue<T>(id, d);
        }
        
        template <typename T = bool>
        T set(std::string id, T d) {
            return geode::Mod::get()->setSavedValue<T>(id, d);
        }

        namespace trajectory {
            extern bool making_trajectory, trajectory_end;
            
            void update(GJBaseGameLayer *gjbgl, PlayerObject *trajectory_players[2], cocos2d::CCDrawNode *trajectory_node);
        }

        namespace frame_stepper {
            extern bool on;
            extern int step_for;
        }
    }
    
    namespace practice_fix {
        #define UV_CP_FIELDS(X) \
            X(m_mainLayer) \
            X(m_wasTeleported) \
            X(m_fixGravityBug) \
            X(m_reverseSync) \
            X(m_yVelocityBeforeSlope) \
            X(m_dashX) \
            X(m_dashY) \
            X(m_dashAngle) \
            X(m_dashStartTime) \
            X(m_dashRing) \
            X(m_slopeStartTime) \
            X(m_justPlacedStreak) \
            X(m_maybeLastGroundObject) \
            X(m_collisionLogTop) \
            X(m_collisionLogBottom) \
            X(m_collisionLogLeft) \
            X(m_collisionLogRight) \
            X(m_lastCollisionBottom) \
            X(m_lastCollisionTop) \
            X(m_lastCollisionLeft) \
            X(m_lastCollisionRight) \
            X(m_unk50C) \
            X(m_unk510) \
            X(m_currentSlope2) \
            X(m_preLastGroundObject) \
            X(m_slopeAngle) \
            X(m_slopeSlidingMaybeRotated) \
            X(m_quickCheckpointMode) \
            X(m_collidedObject) \
            X(m_lastGroundObject) \
            X(m_collidingWithLeft) \
            X(m_collidingWithRight) \
            X(m_maybeSavedPlayerFrame) \
            X(m_scaleXRelated2) \
            X(m_groundYVelocity) \
            X(m_yVelocityRelated) \
            X(m_scaleXRelated3) \
            X(m_scaleXRelated4) \
            X(m_scaleXRelated5) \
            X(m_isCollidingWithSlope) \
            X(m_dashFireSprite) \
            X(m_isBallRotating) \
            X(m_unk669) \
            X(m_currentSlope3) \
            X(m_currentSlope) \
            X(unk_584) \
            X(m_collidingWithSlopeId) \
            X(m_slopeFlipGravityRelated) \
            X(m_particleSystems) \
            X(m_slopeAngleRadians) \
            X(m_rotateObjectsRelated) \
            X(m_maybeRotatedObjectsMap) \
            X(m_rotationSpeed) \
            X(m_rotateSpeed) \
            X(m_isRotating) \
            X(m_isBallRotating2) \
            X(m_hasGlow) \
            X(m_isHidden) \
            X(m_ghostType) \
            X(m_ghostTrail) \
            X(m_iconSprite) \
            X(m_iconSpriteSecondary) \
            X(m_iconSpriteWhitener) \
            X(m_iconGlow) \
            X(m_vehicleSprite) \
            X(m_vehicleSpriteSecondary) \
            X(m_birdVehicle) \
            X(m_vehicleSpriteWhitener) \
            X(m_vehicleGlow) \
            X(m_swingFireMiddle) \
            X(m_swingFireBottom) \
            X(m_swingFireTop) \
            X(m_dashSpritesContainer) \
            X(m_regularTrail) \
            X(m_shipStreak) \
            X(m_waveTrail) \
            X(m_speedMultiplier) \
            X(m_yStart) \
            X(m_gravity) \
            X(m_trailingParticleLife) \
            X(m_unk648) \
            X(m_gameModeChangedTime) \
            X(m_padRingRelated) \
            X(m_maybeReducedEffects) \
            X(m_maybeIsFalling) \
            X(m_shouldTryPlacingCheckpoint) \
            X(m_playEffects) \
            X(m_maybeCanRunIntoBlocks) \
            X(m_hasGroundParticles) \
            X(m_hasShipParticles) \
            X(m_isOnGround3) \
            X(m_checkpointTimeout) \
            X(m_lastCheckpointTime) \
            X(m_lastJumpTime) \
            X(m_lastFlipTime) \
            X(m_flashTime) \
            X(m_flashRelated) \
            X(m_flashRelated1) \
            X(m_colorRelated2) \
            X(m_flashRelated3) \
            X(m_lastSpiderFlipTime) \
            X(m_unkBool5) \
            X(m_maybeIsVehicleGlowing) \
            X(m_switchWaveTrailColor) \
            X(m_practiceDeathEffect) \
            X(m_accelerationOrSpeed) \
            X(m_snapDistance) \
            X(m_ringJumpRelated) \
            X(m_ringRelatedSet) \
            X(m_objectSnappedTo) \
            X(m_pendingCheckpoint) \
            X(m_onFlyCheckpointTries) \
            X(m_robotSprite) \
            X(m_spiderSprite) \
            X(m_maybeSpriteRelated) \
            X(m_playerGroundParticles) \
            X(m_trailingParticles) \
            X(m_shipClickParticles) \
            X(m_vehicleGroundParticles) \
            X(m_ufoClickParticles) \
            X(m_robotBurstParticles) \
            X(m_dashParticles) \
            X(m_swingBurstParticles1) \
            X(m_swingBurstParticles2) \
            X(m_useLandParticles0) \
            X(m_landParticles0) \
            X(m_landParticles1) \
            X(m_landParticlesAngle) \
            X(m_landParticleRelatedY) \
            X(m_playerStreak) \
            X(m_streakStrokeWidth) \
            X(m_disableStreakTint) \
            X(m_alwaysShowStreak) \
            X(m_shipStreakType) \
            X(m_slopeRotation) \
            X(m_currentSlopeYVelocity) \
            X(m_unk3d0) \
            X(m_blackOrbRelated) \
            X(m_unk3e0) \
            X(m_unk3e1) \
            X(m_isAccelerating) \
            X(m_isCurrentSlopeTop) \
            X(m_collidedTopMinY) \
            X(m_collidedBottomMaxY) \
            X(m_collidedLeftMaxX) \
            X(m_collidedRightMinX) \
            X(m_fadeOutStreak) \
            X(m_canPlaceCheckpoint) \
            X(m_colorRelated) \
            X(m_secondColorRelated) \
            X(m_hasCustomGlowColor) \
            X(m_glowColor) \
            X(m_maybeIsColliding) \
            X(m_jumpBuffered) \
            X(m_stateRingJump) \
            X(m_wasJumpBuffered) \
            X(m_wasRobotJump) \
            X(m_stateJumpBuffered) \
            X(m_stateRingJump2) \
            X(m_touchedRing) \
            X(m_touchedCustomRing) \
            X(m_touchedGravityPortal) \
            X(m_maybeTouchedBreakableBlock) \
            X(m_jumpRelatedAC2) \
            X(m_touchedPad) \
            X(m_yVelocity) \
            X(m_fallSpeed) \
            X(m_isOnSlope) \
            X(m_wasOnSlope) \
            X(m_slopeVelocity) \
            X(m_maybeUpsideDownSlope) \
            X(m_isShip) \
            X(m_isBird) \
            X(m_isBall) \
            X(m_isDart) \
            X(m_isRobot) \
            X(m_isSpider) \
            X(m_isUpsideDown) \
            X(m_isDead) \
            X(m_isOnGround) \
            X(m_isGoingLeft) \
            X(m_isSideways) \
            X(m_isSwing) \
            X(m_reverseRelated) \
            X(m_maybeReverseSpeed) \
            X(m_maybeReverseAcceleration) \
            X(m_xVelocityRelated2) \
            X(m_isDashing) \
            X(m_unk9e8) \
            X(m_groundObjectMaterial) \
            X(m_vehicleSize) \
            X(m_playerSpeed) \
            X(m_shipRotation) \
            X(m_lastPortalPos) \
            X(m_unkUnused3) \
            X(m_isOnGround2) \
            X(m_lastLandTime) \
            X(m_platformerVelocityRelated) \
            X(m_maybeIsBoosted) \
            X(m_scaleXRelatedTime) \
            X(m_decreaseBoostSlide) \
            X(m_unkA29) \
            X(m_isLocked) \
            X(m_controlsDisabled) \
            X(m_lastGroundedPos) \
            X(m_touchingRings) \
            X(m_touchedRings) \
            X(m_lastActivatedPortal) \
            X(m_hasEverJumped) \
            X(m_hasEverHitRing) \
            X(m_playerColor1) \
            X(m_playerColor2) \
            X(m_position) \
            X(m_isSecondPlayer) \
            X(m_unkA99) \
            X(m_totalTime) \
            X(m_isBeingSpawnedByDualPortal) \
            X(m_unkAAC) \
            X(m_unkAngle1) \
            X(m_yVelocityRelated3) \
            X(m_defaultMiniIcon) \
            X(m_swapColors) \
            X(m_switchDashFireColor) \
            X(m_followRelated) \
            X(m_playerFollowFloats) \
            X(m_unk838) \
            X(m_stateOnGround) \
            X(m_stateUnk) \
            X(m_stateNoStickX) \
            X(m_stateNoStickY) \
            X(m_stateUnk2) \
            X(m_stateBoostX) \
            X(m_stateBoostY) \
            X(m_maybeStateForce2) \
            X(m_stateScale) \
            X(m_platformerXVelocity) \
            X(m_holdingRight) \
            X(m_holdingLeft) \
            X(m_leftPressedFirst) \
            X(m_scaleXRelated) \
            X(m_maybeHasStopped) \
            X(m_xVelocityRelated) \
            X(m_maybeGoingCorrectSlopeDirection) \
            X(m_isSliding) \
            X(m_maybeSlopeForce) \
            X(m_isOnIce) \
            X(m_physDeltaRelated) \
            X(m_isOnGround4) \
            X(m_maybeSlidingTime) \
            X(m_maybeSlidingStartTime) \
            X(m_changedDirectionsTime) \
            X(m_slopeEndTime) \
            X(m_isMoving) \
            X(m_platformerMovingLeft) \
            X(m_platformerMovingRight) \
            X(m_isSlidingRight) \
            X(m_maybeChangedDirectionAngle) \
            X(m_unkUnused2) \
            X(m_isPlatformer) \
            X(m_stateNoAutoJump) \
            X(m_stateDartSlide) \
            X(m_stateHitHead) \
            X(m_stateFlipGravity) \
            X(m_gravityMod) \
            X(m_stateForce) \
            X(m_stateForceVector) \
            X(m_affectedByForces) \
            X(m_jumpPadRelated) \
            X(m_somethingPlayerSpeedTime) \
            X(m_playerSpeedAC) \
            X(m_fixRobotJump) \
            X(m_holdingButtons) \
            X(m_inputsLocked) \
            X(m_currentRobotAnimation) \
            X(m_gv0123) \
            X(m_iconRequestID) \
            X(m_robotBatchNode) \
            X(m_spiderBatchNode) \
            X(m_unk958) \
            X(m_robotFire) \
            X(m_unkUnused) \
            X(m_gameLayer) \
            X(m_parentLayer) \
            X(m_actionManager) \
            X(m_isOutOfBounds) \
            X(m_fallStartY) \
            X(m_disablePlayerSqueeze) \
            X(m_robotAnimation1Enabled) \
            X(m_robotAnimation2Enabled) \
            X(m_spiderAnimationEnabled) \
            X(m_ignoreDamage) \
            X(m_enable22Changes)

        struct checkpoint_data_base {};

        #define UV_CP_HOLDER(name) \
            template <class P, class = void> struct holder_##name {}; \
            template <class P> struct holder_##name<P, std::void_t<decltype(std::declval<P&>().name)>> { \
                std::remove_cvref_t<decltype(std::declval<P&>().name)> name{}; \
            };
        UV_CP_FIELDS(UV_CP_HOLDER)
        #undef UV_CP_HOLDER

        #define UV_CP_BASE(name) , holder_##name<PlayerObject>
        struct checkpoint_data : checkpoint_data_base UV_CP_FIELDS(UV_CP_BASE) {
            double rotation = 0.0;
        };
        #undef UV_CP_BASE

        void log_missing_fields(void);
        checkpoint_data from_playerobject(PlayerObject *po);
        void restore_playerobject(PlayerObject *po, checkpoint_data cpd);
    }
}
