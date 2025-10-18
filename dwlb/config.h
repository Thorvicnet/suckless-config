#define HEX_COLOR(hex)                                                         \
  {.red = ((hex >> 24) & 0xff) * 257,                                          \
   .green = ((hex >> 16) & 0xff) * 257,                                        \
   .blue = ((hex >> 8) & 0xff) * 257,                                          \
   .alpha = (hex & 0xff) * 257}

// use ipc functionality
static bool ipc = true;
// initially hide all bars
static bool hidden = false;
// initially draw all bars at the bottom
static bool bottom = false;
// hide vacant tags
static bool hide_vacant = false;
// vertical pixel padding above and below text
static uint32_t vertical_padding = 1;
// allow in-line color commands in status text
static bool status_commands = true;
// center title text
static bool center_title = false;
// use title space as status text element
static bool custom_title = true;
// title color use active colors
static bool active_color_title = true;
// scale
static uint32_t buffer_scale = 1;
// tag names
static char *tags_names[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};

// font
static char *fontstr = "FiraCode Nerd Font:size=12";

// set 16-bit colors for bar (RRGGBB + FF alpha)
static pixman_color_t active_fg_color = HEX_COLOR(0xE1E3E4ff);
static pixman_color_t active_bg_color = HEX_COLOR(0x424B5Bff); // bg4

static pixman_color_t occupied_fg_color = HEX_COLOR(0xE1E3E4ff);
static pixman_color_t occupied_bg_color = HEX_COLOR(0x373C4Bff); // bg2

static pixman_color_t inactive_fg_color = HEX_COLOR(0x828A9Aff); // gray
static pixman_color_t inactive_bg_color = HEX_COLOR(0x333846ff); // bg1

static pixman_color_t urgent_fg_color = HEX_COLOR(0x181A1Cff); // black
static pixman_color_t urgent_bg_color = HEX_COLOR(0xFF6D7Eff); // bg_red

static pixman_color_t middle_bg_color = HEX_COLOR(0x24272Eff);
static pixman_color_t middle_bg_color_selected = HEX_COLOR(0x24272Eff);
