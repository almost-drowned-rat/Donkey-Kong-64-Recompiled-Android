#include <algorithm>
#ifdef __ANDROID__
#include <array>
#include <memory>
#include "config/ui_config_page_options_menu.h"
#include "composites/ui_mod_menu.h"
#include "composites/ui_mod_discovery.h"
#include "elements/ui_button.h"
#endif
#include "donk_config.h"
#include "recompui/recompui.h"
#include "recompui/config.h"
#include "recompinput/recompinput.h"
#include "donk_sound.h"
#include "donk_draw.h"
#include "donk_support.h"
#include "ultramodern/config.hpp"
#include "librecomp/files.hpp"
#include "librecomp/config.hpp"
#include "util/file.h"
#include <filesystem>
#include <fstream>
#include <iomanip>

#if defined(_WIN32)
#include <Shlobj.h>
#elif defined(__linux__)
#include <unistd.h>
#include <pwd.h>
#elif defined(__APPLE__)
#include "apple/rt64_apple.h"
#endif

static void add_general_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;

    static EnumOptionVector camera_invert_mode_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
    };
    config.add_enum_option(
        dk64::configkeys::general::third_person_camera_invert_mode,
        "Invert Camera",
        "Inverts the camera controls for the third person camera if it's enabled.",
        camera_invert_mode_options,
        dk64::CameraInvertMode::InvertNone
    );
    static EnumOptionVector first_person_invert_mode_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {dk64::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {dk64::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        dk64::configkeys::general::first_person_invert_mode,
        "Invert First Person View",
        "Inverts the camera controls in first person view. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        first_person_invert_mode_options,
        dk64::CameraInvertMode::InvertY
    );
    static EnumOptionVector gyro_invert_mode_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {dk64::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {dk64::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        dk64::configkeys::general::gyro_invert_mode,
        "Invert Gyro",
        "Inverts gyro controls.",
        gyro_invert_mode_options,
        dk64::CameraInvertMode::InvertY
    );

    static EnumOptionVector mouse_invert_mode_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {dk64::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {dk64::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        dk64::configkeys::general::mouse_invert_mode,
        "Invert Mouse",
        "Inverts Mouse controls.",
        mouse_invert_mode_options,
        dk64::CameraInvertMode::InvertY
    );
    static EnumOptionVector swimming_invert_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {dk64::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {dk64::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        dk64::configkeys::general::swimming_invert_mode,
        "Invert Swimming",
        "Inverts the controls for swimming. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        swimming_invert_options,
        dk64::CameraInvertMode::InvertY
    );
    // Story Skip
    static EnumOptionVector story_skip_options = {
        {dk64::StorySkipMode::Off, "Off", "None"},
        {dk64::StorySkipMode::IntroStory, "IntroStory", "Intro Story Only"},
        {dk64::StorySkipMode::VanillaOn, "VanillaOn", "On"}
    };
    config.add_enum_option(
        dk64::configkeys::general::story_skip,
        "Story Skip",
        "Skips some story cutscenes.<br /><recomp-color primary>Off</recomp-color>: No story cutscenes are skipped.<br /><recomp-color primary>Intro Story Only</recomp-color>: Only the 5-minute introductory story is skipped. The K. Rool level intros will still play.<br /><recomp-color primary>On</recomp-color>: Both the introductory story and all K. Rool level intros are skipped.",
        story_skip_options,
        dk64::StorySkipMode::Off
    );
    // Camera Type
    static EnumOptionVector camera_type_options = {
        {dk64::CameraTypeMode::Free, "Free", "Free Cam"},
        {dk64::CameraTypeMode::Follow, "Follow", "Follow Cam"},
        {dk64::CameraTypeMode::BetterFree, "BetterFree", "Better Free Cam"},
        {dk64::CameraTypeMode::Analog, "Analog", "Analog Camera"}
    };
    config.add_enum_option(
        dk64::configkeys::general::camera_type,
        "Camera Type",
        "Changes the camera behavior.<br /><recomp-color primary>Free Cam</recomp-color>: Camera can be controlled via pressing C-Left and C-Right. The camera does not try to push itself behind the player.<br /><recomp-color primary>Follow Cam</recomp-color>: Similar to <recomp-color secondary>Free Cam</recomp-color>, but the camera tries to push itself behind the player.<br /><recomp-color primary>Better Free Cam</recomp-color>: Similar to <recomp-color secondary>Free Cam</recomp-color>, but instead of a button press turning the camera 45 degrees, holding the button moves the camera at 5 degrees per frame.<br /><recomp-color primary>Analog Camera</recomp-color>: Control of the camera is controlled by your right analogue stick. It is advised that C-Up is remapped to something outside your right analogue stick in order to have a good time with this.",
        camera_type_options,
        dk64::CameraTypeMode::Free
    );
    config.add_number_option(
        dk64::configkeys::general::analog_camera_sensitivity,
        "Analog Camera Sensitivity",
        "Sets the sensitivity of the right stick analog camera, if enabled.",
        1, 10, 1, 0, false, 3
    );
    config.add_option_hidden_dependency(
        dk64::configkeys::general::analog_camera_sensitivity,
        dk64::configkeys::general::camera_type,
        dk64::CameraTypeMode::Free,
        dk64::CameraTypeMode::Follow,
        dk64::CameraTypeMode::BetterFree
    );
    // Lightning Flashes
    static EnumOptionVector lightning_flash_options = {
        {dk64::LightningFlashMode::Off, "Off", "Off"},
        {dk64::LightningFlashMode::Reduced, "Reduced", "Reduced"},
        {dk64::LightningFlashMode::Vanilla, "Vanilla", "Vanilla"}
    };
    config.add_enum_option(
        dk64::configkeys::general::lightning_flashes,
        "Lightning Flash Frequency",
        "Changes the frequency of lightning flashes within the game.<br /><recomp-color primary>Vanilla</recomp-color>: Lightning flashes at vanilla frequency<br /><recomp-color primary>Reduced</recomp-color>: Lightning flashes at 10x as less<br /><recomp-color primary>Off</recomp-color>: Lightning flashes are completely disabled",
        lightning_flash_options,
        dk64::LightningFlashMode::Reduced
    );
    
}

template <typename T = uint32_t>
T get_general_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_general_config().get_option_value(option_id)));
}

template <typename T = uint32_t>
T get_general_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_general_config().get_option_value(option_id)));
}

dk64::CameraInvertMode dk64::get_camera_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::camera_invert_mode);
}

dk64::CameraInvertMode dk64::get_third_person_camera_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::third_person_camera_invert_mode);
}

dk64::CameraInvertMode dk64::get_swimming_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::swimming_invert_mode);
}

dk64::StorySkipMode dk64::get_story_skip() {
    return get_general_config_enum_value<dk64::StorySkipMode>(dk64::configkeys::general::story_skip);
}

dk64::CameraTypeMode dk64::get_camera_type() {
    return get_general_config_enum_value<dk64::CameraTypeMode>(dk64::configkeys::general::camera_type);
}

dk64::LightningFlashMode dk64::get_lightning_flash() {
    return get_general_config_enum_value<dk64::LightningFlashMode>(dk64::configkeys::general::lightning_flashes);
}

dk64::CameraInvertMode dk64::get_first_person_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::first_person_invert_mode);
}

dk64::CameraInvertMode dk64::get_gyro_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::gyro_invert_mode);
}

dk64::CameraInvertMode dk64::get_mouse_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::mouse_invert_mode);
}

uint32_t dk64::get_analog_cam_sensitivity() {
    return get_general_config_number_value(dk64::configkeys::general::analog_camera_sensitivity);
}

template <typename T = uint32_t>
T get_graphics_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_graphics_config().get_option_value(option_id)));
}

template <typename T = uint32_t>
T get_graphics_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_graphics_config().get_option_value(option_id)));
}

template <typename T = uint32_t>
T get_technical_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_config("technical").get_option_value(option_id)));
}

static void add_sound_options(recomp::config::Config &config) {
    config.add_percent_number_option(
        dk64::configkeys::sound::bgm_volume,
        "Background Music Volume",
        "Controls the overall volume of background music.",
        100.0f
    );
    config.add_percent_number_option(
        dk64::configkeys::sound::sfx_volume,
        "SFX Volume",
        "Controls the overall volume of sound effects.",
        100.0f
    );
}
template <typename T = uint32_t>
T get_sound_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_sound_config().get_option_value(option_id)));
}

int dk64::get_bgm_volume() {
    return get_sound_config_number_value<int>(dk64::configkeys::sound::bgm_volume);
}

int dk64::get_sfx_volume() {
    return get_sound_config_number_value<int>(dk64::configkeys::sound::sfx_volume);
}

static void add_graphics_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;

    // Cutscene borders
    static EnumOptionVector cutscene_border_options = {
        {dk64::CutsceneBordersMode::Off, "Off", "Off"},
        {dk64::CutsceneBordersMode::On, "On", "On"}
    };
    config.add_enum_option(
        dk64::configkeys::graphics::cutscene_borders,
        "Cutscene Borders",
        "If turned on, cutscenes will show borders on the top and the bottom of the screen as in the vanilla game.",
        cutscene_border_options,
        dk64::CutsceneBordersMode::On
    );
    config.add_percent_number_option(
        dk64::configkeys::graphics::draw_distance,
        "Draw Distance",
        "Controls the draw distance within the game from 0% (vanilla) to 100% (10x vanilla). Some objects do not get a draw distance increase for technical reasons.",
        0.0f
    );
}

dk64::CutsceneBordersMode dk64::get_cutscene_borders() {
    return get_graphics_config_enum_value<dk64::CutsceneBordersMode>(dk64::configkeys::graphics::cutscene_borders);
}

int dk64::get_draw_distance() {
    return get_graphics_config_number_value<int>(dk64::configkeys::graphics::draw_distance);
}

static void add_technical_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;
    // Multiplayer
    static EnumOptionVector multiplayer_options = {
        {dk64::MultiplayerEnabled::Off, "Off", "Off"},
        {dk64::MultiplayerEnabled::On, "On", "On"}
    };
    config.add_enum_option(
        dk64::configkeys::technical::multiplayer_enabled,
        "Enable Multiplayer",
        "Enables the ability to enter Multiplayer. Multiplayer is considered a <recomp-color secondary>Work-in-Progress feature</recomp-color> that is classed as non-functional.<br />You should not enable this feature unless you wish to use glitches such as \"Funky Weapons Glitch\" or \"Main Menu Moves\"",
        multiplayer_options,
        dk64::MultiplayerEnabled::Off
    );
}

dk64::MultiplayerEnabled dk64::get_multiplayer_enabled() {
    return get_technical_config_enum_value<dk64::MultiplayerEnabled>(dk64::configkeys::technical::multiplayer_enabled);
}

static void set_control_defaults() {
    using namespace recompinput;

    // Left shoulder -> C Down | Backwards eggs / zoom out
    set_default_mapping_for_controller(
        GameInput::C_DOWN,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, true),
            InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
        }
    );

    // Right shoulder -> C Up | Forwards eggs / first person
    set_default_mapping_for_controller(
        GameInput::C_UP,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, false),
            InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
        }
    );

    // North button -> C Left | Talon trot / camera left
    set_default_mapping_for_controller(
        GameInput::C_LEFT,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, false),
            InputField::controller_digital(SDL_CONTROLLER_BUTTON_NORTH)
        }
    );

    // East button -> C Right | Wonderwing / camera right
    set_default_mapping_for_controller(
        GameInput::C_RIGHT,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, true),
            InputField::controller_digital(SDL_CONTROLLER_BUTTON_EAST)
        }
    );

    // R3 -> L | Unused in BK but can be used in mods
    set_default_mapping_for_controller(GameInput::L, { InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK) });
}

static void set_control_descriptions() {
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_POS, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_NEG, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_NEG, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_POS, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::A, "Used to jump and select options in menus.");
    recompinput::set_game_input_description(recompinput::GameInput::B, "Used for attacks, which change depending on whether you are stationary, moving, in the air, or crouching.");
    recompinput::set_game_input_description(recompinput::GameInput::Z, "Used to crouch, which enables A, B and the C-Buttons to perform different actions.");
    recompinput::set_game_input_description(recompinput::GameInput::L, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::R, "Used to center the camera behind the player, and to perform tighter turns while in the coconut boat.");
    recompinput::set_game_input_description(recompinput::GameInput::START, "Used for pausing and for skipping certain cutscenes.");
    recompinput::set_game_input_description(recompinput::GameInput::C_UP, "Used to enter first-person mode and to play your instrument while holding Z");
    recompinput::set_game_input_description(recompinput::GameInput::C_DOWN, "Used to toggle between the different camera zoom levels, and to pull out the fairy camera while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_LEFT, "Used to rotate the camera sideways. Axis inversion can be configured in the General tab. Also used to pull out your fruit weapon while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_RIGHT, "Used to rotate the camera sideways. Axis inversion can be configured in the General tab). Also used to throw an orange while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_UP, "Used to move in Jetpac and Donkey Kong arcade games. Mods may also use it separately for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_DOWN, "Used to move in Jetpac and Donkey Kong arcade games. Mods may also use it separately for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_LEFT, "Used to move in Jetpac and Donkey Kong arcade games. Mods may also use it separately for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_RIGHT, "Used to move in Jetpac and Donkey Kong arcade games. Mods may also use it separately for additional features.");
}

#ifdef __ANDROID__
// Android: Settings > More - Technical, Manage Mods and Find Mods in one tab (their own tabs are hidden). The tab
// lists the three; choosing one opens it as a sub-page with a Back button to the list. The pages are the ones of
// the hidden tabs, made once when the tab opens and shown one at a time. (Replaced by the card menu below.)
[[maybe_unused]] static void create_more_tab() {
    using namespace recompui;
    const bool discovery = !get_discovery_url().empty() && !ultramodern::is_game_started();
    config::set_tab_visible("technical", false);
    config::set_tab_visible(config::mods::id, false);
    if (discovery) {
        config::set_tab_visible(config::mod_discovery::id, false);
    }
    config::create_tab("More", "more",
        [discovery](ContextId context, Element* parent) {
            struct Pages {
                Element* list = nullptr;
                std::array<Element*, 3> page{};
            };
            auto pages = std::make_shared<Pages>();
            auto box = [&](Element* where) {
                Element* e = context.create_element<Element>(where);
                e->set_width(100, Unit::Percent);
                e->set_height(100, Unit::Percent);
                e->set_display(Display::Flex);
                e->set_flex_direction(FlexDirection::Column);
                return e;
            };
            auto show = [pages](int which) {   // -1: the list
                pages->list->set_display(which < 0 ? Display::Flex : Display::None);
                for (int i = 0; i < 3; i++) {
                    if (pages->page[i] != nullptr) {
                        pages->page[i]->set_display(i == which ? Display::Flex : Display::None);
                    }
                }
            };
            pages->list = box(parent);
            pages->list->set_padding(32.0f);
            pages->list->set_gap(16.0f);
            const char* names[3] = { "Technical", "Manage Mods", "Find Mods" };
            for (int i = 0; i < 3; i++) {
                if (i == 2 && !discovery) {
                    continue;
                }
                Button* open = context.create_element<Button>(pages->list, names[i], ButtonStyle::Secondary, ButtonSize::Medium);
                open->set_width(480.0f);
                open->add_pressed_callback([show, i]() { show(i); });

                Element* page = box(parent);
                Button* back = context.create_element<Button>(page, std::string("\u2039  Back"), ButtonStyle::Tertiary, ButtonSize::Small);
                back->set_width(160.0f);
                back->set_margin_bottom(8.0f);
                back->add_pressed_callback([show]() { show(-1); });
                Element* body = context.create_element<Element>(page);
                body->set_flex_grow(1.0f);
                body->set_width(100, Unit::Percent);
                if (i == 0) {
                    context.create_element<ConfigPageOptionsMenu>(body, &config::get_config("technical"), true);
                } else if (i == 1) {
                    context.create_element<ModMenu>(body);
                } else {
                    context.create_element<ModDownloadsPanel>(body);
                }
                page->set_display(Display::None);
                pages->page[i] = page;
            }
        },
        // Can close: unapplied Technical options ask first (as their own tab did)
        [](TabCloseContext close_context) {
            recomp::config::Config& technical = config::get_config("technical");
            if (technical.requires_confirmation && technical.is_dirty()) {
                open_choice_prompt(
                    "Technical options have unapplied changes.", "Would you like to apply or discard the changes?",
                    "Apply", "Discard",
                    [close_context]() {
                        config::get_config("technical").save_config();
                        if (close_context == TabCloseContext::ModalClose) config::close();
                    },
                    [close_context]() {
                        config::get_config("technical").revert_temp_config();
                        if (close_context == TabCloseContext::ModalClose) config::close();
                    },
                    ButtonStyle::Success, ButtonStyle::Danger, true);
                return false;
            }
            return true;
        },
        [](TabCloseContext) {}
    );
}
#endif

#ifdef __ANDROID__
// the GPU driver card (src/main/main.cpp)
recompui::config::MenuCardEntry android_driver_card();

static void create_android_card_tabs() {
    using namespace recompui::config;
    namespace gk = dk64::configkeys::general;
    set_java_menu_class("com.deivid22srk.dk64recomp.TouchControls");
    set_menu_accent(249, 99, 24);   // the start screen's colour (the orange sky)
    create_card_tab("General", "card_general", []() {
        std::vector<MenuCardEntry> l;
        l.push_back(header_card("Session"));
        MenuCardEntry back;
        back.label = "Back to game";
        back.run = []() { recompui::config::close(); };
        l.push_back(back);
        for (auto &e : java_menu_cards("game")) l.push_back(e);                 // Exit game
        l.push_back(header_card("Gameplay"));
        l.push_back(option_card(general::id, gk::story_skip));
        l.push_back(option_card(general::id, gk::lightning_flashes));
        l.push_back(option_card("technical", dk64::configkeys::technical::multiplayer_enabled));
        l.push_back(header_card("Input"));
        l.push_back(option_card(general::id, general::options::background_input_mode));
        return l;
    });
    create_card_tab("Camera", "card_camera", []() {
        return std::vector<MenuCardEntry>{
            header_card("Camera"),
            option_card(general::id, gk::camera_type),
            option_card(general::id, gk::analog_camera_sensitivity),
            header_card("Invert"),
            option_card(general::id, gk::third_person_camera_invert_mode),
            option_card(general::id, gk::first_person_invert_mode),
            option_card(general::id, gk::swimming_invert_mode),
        };
    });
    create_card_tab("Graphics", "card_graphics", []() {
        namespace go = graphics::options;
        namespace dg = dk64::configkeys::graphics;
        std::vector<MenuCardEntry> l;
        std::vector<std::string> used;
        auto add = [&](const std::string &id) {
            l.push_back(option_card(graphics::id, id));
            used.push_back(id);
        };
        l.push_back(header_card("Picture"));
        add(go::res_option);
        add(go::ar_option);
        add(go::hr_option);
        add(dg::cutscene_borders);
        l.push_back(header_card("Quality"));
        add(go::ds_option);
        add(go::msaa_option);
        add(dg::draw_distance);
        // any other option of the graphics config (shown only where it isn't hidden)
        auto schema = get_config(graphics::id).get_config_schema();
        for (auto &o : schema.options) {
            if (o.id != go::rr_option && o.id != go::rr_manual_value &&
                std::find(used.begin(), used.end(), o.id) == used.end() && o.type != recomp::config::ConfigOptionType::String) {
                add(o.id);
            }
        }
        l.push_back(header_card("Frame rate"));
        add(go::rr_option);
        add(go::rr_manual_value);
        for (auto &e : java_menu_cards("display")) l.push_back(e);              // FPS
        l.push_back(header_card("Driver"));
        l.push_back(android_driver_card());
        return l;
    }, graphics::id);
    create_card_tab("Shaders", "card_shaders", []() {
        std::vector<MenuCardEntry> l;
        l.push_back(header_card("RetroArch shaders"));
        for (auto &e : java_menu_cards("shader")) l.push_back(e);
        l.push_back(header_card("Picture"));
        MenuCardEntry res = option_card("android_options", "shader_resolution");
        res.hidden = nullptr;                                                     // hidden as an option of its own tab
        l.push_back(res);
        return l;
    });
    create_card_tab("Audio", "card_sound", []() {
        std::vector<MenuCardEntry> l;
        l.push_back(header_card("Volume"));
        for (auto &e : config_cards(sound::id)) l.push_back(e);
        return l;
    });
    create_card_tab("Controls", "card_controls", []() {
        std::vector<MenuCardEntry> l;
        l.push_back(header_card("Controller"));
        l.push_back(bindings_card());
        l.push_back(option_card(general::id, general::options::joystick_deadzone));
        l.push_back(option_card(general::id, general::options::rumble_strength));
        l.push_back(header_card("Gyro and mouse"));
        l.push_back(option_card(general::id, general::options::gyro_sensitivity));
        l.push_back(option_card(general::id, gk::gyro_invert_mode));
        l.push_back(option_card(general::id, general::options::mouse_sensitivity));
        l.push_back(option_card(general::id, gk::mouse_invert_mode));
        l.push_back(header_card("Touch controls"));
        for (auto &e : java_menu_cards("touch")) l.push_back(e);
        return l;
    });
}
#endif

void dk64::init_config() {
    std::filesystem::path recomp_dir = recompui::file::get_app_folder_path();

    if (!recomp_dir.empty()) {
        std::filesystem::create_directories(recomp_dir);
    }

    recompui::config::GeneralTabOptions general_options{};
    general_options.has_rumble_strength = true;
    general_options.has_gyro_sensitivity = true;
    general_options.has_mouse_sensitivity = true;

    auto &general_config = recompui::config::create_general_tab(general_options);
    add_general_options(general_config);

    auto &graphics_config = recompui::config::create_graphics_tab();
    add_graphics_options(graphics_config);

    set_control_defaults();
    set_control_descriptions();
#ifdef __ANDROID__
    // Settings > Android Options (after Graphics): the shader resolution; main.cpp adds the buttons for the shader
    // pages of the menu Back opens and the GPU driver.
    {
        auto &android_config = recompui::config::create_config_tab("Android Options", "android_options", false);
        using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;
        static EnumOptionVector shader_resolution_options = {
            {0, "Original", "Original"},
            {1, "2x", "2x"},
            {2, "Full", "Full"}
        };
        android_config.add_enum_option(
            "shader_resolution",
            "Shader Resolution",
            "The size of the game's picture a RetroArch shader works on. "
            "<recomp-color primary>Original</recomp-color>: the N64's 240 lines, as in RetroArch - the look the shaders "
            "were made for (scanlines, CRT masks, pixel upscalers) and by far the lightest. "
            "<recomp-color primary>2x</recomp-color>: twice that. "
            "<recomp-color primary>Full</recomp-color>: the window's size - heavy shaders get slow.<br /><br />"
            "<b>Shader...</b> imports shader packs (.zip) and picks a preset, <b>Shader parameters...</b> changes "
            "the preset's settings, <b>GPU Driver (Turnip)...</b> installs a Vulkan driver (the app restarts).",
            shader_resolution_options,
            0u,
            true   // shown as a button (src/main/main.cpp)
        );
    }
#endif
    recompui::config::create_controls_tab();

    auto &sound_config = recompui::config::create_sound_tab();
    add_sound_options(sound_config);

    auto &technical_config = recompui::config::create_config_tab("Technical", "technical", true);
    add_technical_options(technical_config);

#ifdef __ANDROID__
    // The Android card menu (the same in all N64 ports): the configs stay as they are (saved settings keep their
    // files), their own tabs are hidden and card tabs show their options by topic. More / Technical / Android
    // Options are gone; the menu Back opened (Touch, Shaders, FPS, Exit) is in here too (TouchControls' bridge).
    for (const std::string &id : { recompui::config::general::id, recompui::config::graphics::id, std::string("android_options"),
                                   recompui::config::controls::id, recompui::config::sound::id, std::string("technical") }) {
        recompui::config::set_tab_visible(id, false);
    }
    create_android_card_tabs();
#endif
    recompui::config::create_mods_tab();
#ifdef __ANDROID__
    // Manage Mods + Find Mods: one card tab "Mods"
    recompui::config::set_tab_visible(recompui::config::mods::id, false);
    if (!recompui::get_discovery_url().empty() && !ultramodern::is_game_started()) {
        recompui::config::set_tab_visible(recompui::config::mod_discovery::id, false);
    }
    recompui::config::create_card_tab("Mods", "card_mods", []() {
        std::vector<recompui::config::MenuCardEntry> l;
        l.push_back(recompui::config::header_card("Mods"));
        for (auto &e : recompui::config::mods_cards()) l.push_back(e);
        return l;
    });
#endif

    recompui::config::finalize();

}
