#include "main.h"

#ifdef __WIN32__
#define UTOX_RGB(r, g, b) ( ((b)<<16) | ((g)<<8) | (r) )
#else
#define UTOX_RGB(r, g, b) ( ((r)<<16) | ((g)<<8) | (b) )
#endif

void theme_load(char loadtheme)
{
    // Update the settings dropdown UI
    dropdown_theme.selected = loadtheme;

    // ==== Default theme ====
    //---- Main chat area ----
    COLOR_MAIN_BACKGROUND                   = UTOX_RGB(255, 255, 255);
    COLOR_MAIN_TEXT                         = UTOX_RGB(51, 51, 51);
    COLOR_MAIN_SUBTEXT                      = UTOX_RGB(89, 89, 89);
    COLOR_MAIN_ACTIONTEXT                   = UTOX_RGB(68, 68, 255);
    COLOR_MAIN_QUOTETEXT                    = UTOX_RGB(0, 136, 0);
    COLOR_MAIN_URLTEXT                      = UTOX_RGB(78, 166, 234);

    //---- Top & bottom menu colours ----
    COLOR_MENU_BACKGROUND                   = UTOX_RGB(28, 28, 28);
    COLOR_MENU_TEXT                         = UTOX_RGB(192, 192, 192);
    COLOR_MENU_HOVER_BACKGROUND             = UTOX_RGB(32, 32, 32);
    COLOR_MENU_ACTIVE_BACKGROUND            = UTOX_RGB(64, 64, 64);
    COLOR_MENU_ACTIVE_TEXT                  = UTOX_RGB(229, 229, 229);

    //---- Friend list  ----
    COLOR_LIST_BACKGROUND                   = UTOX_RGB(40, 40, 40);
    COLOR_LIST_HOVER_BACKGROUND             = UTOX_RGB(85, 85, 85);
    COLOR_LIST_TEXT                         = COLOR_MAIN_BACKGROUND;
    COLOR_LIST_SUBTEXT                      = UTOX_RGB(186, 186, 186);

    //---- Text selection ----
    COLOR_SELECTION_BACKGROUND              = COLOR_MAIN_TEXT;
    COLOR_SELECTION_TEXT                    = COLOR_MAIN_BACKGROUND;
    COLOR_GROUP_UNUSUAL                     = COLOR_MAIN_URLTEXT;

    //---- Inputs, dropdowns & tooltips ----
    COLOR_EDGE_NORMAL                       = UTOX_RGB(192, 192, 192);
    COLOR_EDGE_ACTIVE                       = COLOR_MAIN_URLTEXT;
    COLOR_EDGE_HOVER                        = UTOX_RGB(112, 112, 112);
    COLOR_ACTIVEOPTION_BACKGROUND           = UTOX_RGB(204, 204, 255);
    COLOR_ACTIVEOPTION_TEXT                 = COLOR_MAIN_TEXT;

    //---- Status circles ----
    COLOR_STATUS_ONLINE                     = UTOX_RGB(107, 194, 96);
    COLOR_STATUS_AWAY                       = UTOX_RGB(206, 191, 69);
    COLOR_STATUS_BUSY                       = UTOX_RGB(200, 78, 78);

    //---- Buttons ---
    COLOR_BUTTON_SUCCESS_BACKGROUND         = COLOR_STATUS_ONLINE;
    COLOR_BUTTON_SUCCESS_TEXT               = COLOR_MAIN_BACKGROUND;
    COLOR_BUTTON_SUCCESS_HOVER_BACKGROUND   = UTOX_RGB(118, 213, 106);
    COLOR_BUTTON_SUCCESS_HOVER_TEXT         = COLOR_MAIN_BACKGROUND;
    COLOR_BUTTON_WARNING_BACKGROUND         = COLOR_STATUS_AWAY;
    COLOR_BUTTON_WARNING_TEXT               = COLOR_MAIN_BACKGROUND;
    COLOR_BUTTON_WARNING_HOVER_BACKGROUND   = UTOX_RGB(227, 210, 76);
    COLOR_BUTTON_WARNING_HOVER_TEXT         = COLOR_MAIN_BACKGROUND;
    COLOR_BUTTON_DANGER_BACKGROUND          = COLOR_STATUS_BUSY;
    COLOR_BUTTON_DANGER_TEXT                = COLOR_MAIN_BACKGROUND;
    COLOR_BUTTON_DANGER_HOVER_BACKGROUND    = UTOX_RGB(220, 86, 86);
    COLOR_BUTTON_DANGER_HOVER_TEXT          = COLOR_MAIN_BACKGROUND;
    COLOR_BUTTON_DISABLED_BACKGROUND        = UTOX_RGB(209, 209, 209);
    COLOR_BUTTON_DISABLED_TEXT              = COLOR_MAIN_BACKGROUND;

    switch (loadtheme) {
    case THEME_DARK:
        COLOR_MAIN_BACKGROUND                   = UTOX_RGB(51, 51, 51);
        COLOR_MAIN_TEXT                         = UTOX_RGB(223, 223, 223);
        COLOR_MAIN_SUBTEXT                      = UTOX_RGB(187, 187, 187);
        COLOR_MAIN_ACTIONTEXT                   = UTOX_RGB(163, 20, 20);
        COLOR_MAIN_URLTEXT                      = COLOR_MAIN_ACTIONTEXT;
        COLOR_MAIN_QUOTETEXT                    = UTOX_RGB(85, 179, 23);
        COLOR_LIST_BACKGROUND                   = UTOX_RGB(34, 34, 34);
        COLOR_LIST_HOVER_BACKGROUND             = UTOX_RGB(21, 21, 21);
        COLOR_MENU_BACKGROUND                   = COLOR_LIST_BACKGROUND;
        COLOR_MENU_HOVER_BACKGROUND             = COLOR_LIST_HOVER_BACKGROUND;
        COLOR_MENU_ACTIVE_BACKGROUND            = COLOR_MAIN_TEXT;
        COLOR_SELECTION_BACKGROUND              = COLOR_MAIN_TEXT;
        COLOR_SELECTION_TEXT                    = COLOR_MAIN_BACKGROUND;
        COLOR_GROUP_UNUSUAL                     = COLOR_MAIN_URLTEXT;
        COLOR_EDGE_NORMAL                       = UTOX_RGB(85, 85, 85);
        COLOR_EDGE_ACTIVE                       = UTOX_RGB(163, 20, 20);
        COLOR_EDGE_HOVER                        = UTOX_RGB(153, 153, 153);
        COLOR_ACTIVEOPTION_BACKGROUND           = UTOX_RGB(163, 20, 20);
        COLOR_ACTIVEOPTION_TEXT                 = COLOR_MAIN_TEXT;
        COLOR_MENU_ACTIVE_TEXT                  = UTOX_RGB(21, 21, 21);
        COLOR_BUTTON_SUCCESS_BACKGROUND         = UTOX_RGB(65, 65, 65);
        COLOR_BUTTON_SUCCESS_TEXT               = UTOX_RGB(51, 166, 61);
        COLOR_BUTTON_SUCCESS_HOVER_BACKGROUND   = UTOX_RGB(69, 81, 71);
        COLOR_BUTTON_SUCCESS_HOVER_TEXT         = UTOX_RGB(110, 255, 58);
        COLOR_BUTTON_WARNING_BACKGROUND         = UTOX_RGB(65, 65, 65);
        COLOR_BUTTON_WARNING_TEXT               = UTOX_RGB(189, 158, 34);
        COLOR_BUTTON_WARNING_HOVER_BACKGROUND   = UTOX_RGB(76, 73, 60);
        COLOR_BUTTON_WARNING_HOVER_TEXT         = UTOX_RGB(255, 141, 42);
        COLOR_BUTTON_DANGER_BACKGROUND          = UTOX_RGB(65, 65, 65);
        COLOR_BUTTON_DANGER_TEXT                = UTOX_RGB(189, 37, 37);
        COLOR_BUTTON_DANGER_HOVER_BACKGROUND    = UTOX_RGB(81, 57, 57);
        COLOR_BUTTON_DANGER_HOVER_TEXT          = UTOX_RGB(250, 38, 38);
        COLOR_BUTTON_DISABLED_BACKGROUND        = UTOX_RGB(65, 65, 65);
        COLOR_BUTTON_DISABLED_TEXT              = COLOR_MAIN_BACKGROUND;
        break;

    case THEME_LIGHT:
        COLOR_LIST_BACKGROUND                   = UTOX_RGB(240, 240, 240);
        COLOR_LIST_HOVER_BACKGROUND             = UTOX_RGB(224, 224, 224);
        COLOR_LIST_TEXT                         = COLOR_MAIN_TEXT;
        COLOR_LIST_SUBTEXT                      = COLOR_MAIN_SUBTEXT;
        COLOR_MENU_BACKGROUND                   = UTOX_RGB(240, 240, 240);
        COLOR_MENU_HOVER_BACKGROUND             = UTOX_RGB(224, 224, 224);
        COLOR_MENU_ACTIVE_BACKGROUND            = UTOX_RGB(85, 85, 85);
        COLOR_MENU_TEXT                         = UTOX_RGB(85, 85, 85);
        COLOR_MENU_ACTIVE_TEXT                  = UTOX_RGB(255, 255, 255);
        COLOR_EDGE_NORMAL                       = UTOX_RGB(192, 192, 192);
        COLOR_EDGE_ACTIVE                       = COLOR_MAIN_URLTEXT;
        COLOR_EDGE_HOVER                        = UTOX_RGB(112, 112, 112);
        COLOR_ACTIVEOPTION_BACKGROUND           = UTOX_RGB(221, 238, 255);
        COLOR_ACTIVEOPTION_TEXT                 = COLOR_MAIN_TEXT;
        break;

    case THEME_HIGHCONTRAST:

        COLOR_MAIN_BACKGROUND                   = UTOX_RGB(255, 255, 255);
        COLOR_MAIN_TEXT                         = UTOX_RGB(0, 0, 1);
        COLOR_MAIN_SUBTEXT                      = COLOR_MAIN_TEXT;
        COLOR_MAIN_ACTIONTEXT                   = UTOX_RGB(0, 0, 51);
        COLOR_MAIN_QUOTETEXT                    = UTOX_RGB(0, 51, 0);
        COLOR_MAIN_URLTEXT                      = COLOR_MAIN_ACTIONTEXT;
        COLOR_MENU_BACKGROUND                   = COLOR_MAIN_BACKGROUND;
        COLOR_MENU_TEXT                         = COLOR_MAIN_TEXT;
        COLOR_MENU_HOVER_BACKGROUND             = COLOR_MAIN_BACKGROUND;
        COLOR_MENU_ACTIVE_BACKGROUND            = COLOR_MAIN_TEXT;
        COLOR_MENU_ACTIVE_TEXT                  = COLOR_MAIN_BACKGROUND;
        COLOR_LIST_BACKGROUND                   = UTOX_RGB(68, 68, 68);
        COLOR_LIST_HOVER_BACKGROUND             = COLOR_MAIN_TEXT;
        COLOR_LIST_TEXT                         = COLOR_MAIN_BACKGROUND;
        COLOR_LIST_SUBTEXT                      = COLOR_MAIN_BACKGROUND;
        COLOR_SELECTION_BACKGROUND              = COLOR_MAIN_TEXT;
        COLOR_SELECTION_TEXT                    = COLOR_MAIN_BACKGROUND;
        COLOR_GROUP_UNUSUAL                     = COLOR_MAIN_URLTEXT;
        COLOR_EDGE_NORMAL                       = COLOR_MAIN_TEXT;
        COLOR_EDGE_ACTIVE                       = COLOR_MAIN_TEXT;
        COLOR_EDGE_HOVER                        = COLOR_MAIN_TEXT;
        COLOR_ACTIVEOPTION_BACKGROUND           = COLOR_MAIN_TEXT;
        COLOR_ACTIVEOPTION_TEXT                 = COLOR_MAIN_BACKGROUND;
        COLOR_STATUS_ONLINE                     = UTOX_RGB(0, 255, 0);
        COLOR_STATUS_AWAY                       = UTOX_RGB(255, 255, 0);
        COLOR_STATUS_BUSY                       = UTOX_RGB(255, 0, 0);
        COLOR_BUTTON_SUCCESS_BACKGROUND         = UTOX_RGB(0, 255, 0);
        COLOR_BUTTON_SUCCESS_TEXT               = COLOR_MAIN_BACKGROUND;
        COLOR_BUTTON_SUCCESS_HOVER_BACKGROUND   = UTOX_RGB(0, 255, 0);
        COLOR_BUTTON_SUCCESS_HOVER_TEXT         = COLOR_MAIN_BACKGROUND;
        COLOR_BUTTON_WARNING_BACKGROUND         = UTOX_RGB(255, 255, 0);
        COLOR_BUTTON_WARNING_TEXT               = COLOR_MAIN_BACKGROUND;
        COLOR_BUTTON_WARNING_HOVER_BACKGROUND   = UTOX_RGB(255, 255, 0);
        COLOR_BUTTON_WARNING_HOVER_TEXT         = COLOR_MAIN_BACKGROUND;
        COLOR_BUTTON_DANGER_BACKGROUND          = UTOX_RGB(255, 0, 0);
        COLOR_BUTTON_DANGER_TEXT                = COLOR_MAIN_BACKGROUND;
        COLOR_BUTTON_DANGER_HOVER_BACKGROUND    = UTOX_RGB(255, 0, 0);
        COLOR_BUTTON_DANGER_HOVER_TEXT          = COLOR_MAIN_BACKGROUND;
        COLOR_BUTTON_DISABLED_BACKGROUND        = UTOX_RGB(68, 68, 68);
        COLOR_BUTTON_DISABLED_TEXT              = COLOR_MAIN_TEXT;
        break;
    }
    status_color[0] = COLOR_STATUS_ONLINE;
    status_color[1] = COLOR_STATUS_AWAY;
    status_color[2] = COLOR_STATUS_BUSY;
    status_color[3] = COLOR_STATUS_BUSY;
}
