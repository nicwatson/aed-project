// FILE: styles.h
//
// Stylesheet string literals for use by style-switching widgets

#ifndef STYLES_H
#define STYLES_H

// Self-test result indicator (checkmark/X)
#define STYLE_INDICATOR_PASS "border-image: url(:/selfTest/Passed.svg);\nbackground-color: rgba(0,0,0,0)"
#define STYLE_INDICATOR_FAIL "border-image: url(:/selfTest/Failed.svg);\nbackground-color: rgba(0,0,0,0)"
#define STYLE_INDICATOR_OFF  "border-image: url(:/selfTest/Off.svg);\nbackground-color: rgba(0,0,0,0)"

// Pictogram-accompanying lamps
#define STYLE_ADVICELAMP_LIT    "background: rgb(248,228,92); border-radius: 8px;"
#define STYLE_ADVICELAMP_UNLIT  "background: rgb(132,130,132); border-radius: 8px;"

// Flash button lighting
#define STYLE_SHOCK_LIT     "image: url(:/shockButton/ShockButton_ButtonPressed.svg); border: 0;"
#define STYLE_SHOCK_UNLIT   "image: url(:/shockButton/ShockButton_Button.svg); border: 0;"

#define FLASH_TIME_DEFAULT 200
#define FLASH_TIME_SHOCK 500


#endif // STYLES_H
