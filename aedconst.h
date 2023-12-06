// FILE aedconst.h
//
// Numerical constants for various AED parameters
// Note: String constants should go in aedGui/strings.h

#ifndef AEDCONST_H
#define AEDCONST_H

// If AEDCONST_DEBUG_ENABLE is defined here, debug values (mostly faster time intervals for timers) will be used instead of normal ones
// COMMENT THIS OUT FOR NORMAL USE
// #define AEDCONST_DEBUG_ENABLE

//
// SELF TEST PARAMETERS
//
    // How long should the test take (ms)?
    #define TEST_TIME 3000

    // Minimum battery level before self-test triggers "CHANGE BATTERIES" error
    #define BATTERY_THRESHHOLD 0.3

//
// STARTUP ADVICE TIMINGS - How long to run each phase?
//
    #define DURATION_UNIT_OK 3000
    #define DURATION_STAY_CALM 3000
    #define DURATION_CHECK_RESP 4000
    #define DURATION_CALL_HELP 4000

//
// AED EVENT STATE PROGRESSION
//
    // Default timer interval for AED to delay between certain events to prevent prompts from being too quicly overwritten
    #define AED_TIMER_DEFAULT 2000

//
// ECG CONSTANTS
//
    // How long should an ECG analysis take?
    #define ECG_TIME 5000

//
// SHOCK MODULE CONSTANTS
//
    // How many joules of total work are available from a full battery pack?
    #define FULL_BATT 800.0
    // Therefore, one joule is (1 / FULL_BATT) worth of battery level (where battery level 1.0 is "full")
    #define BATT_RATIO (1.0 / FULL_BATT)
    // How long does it take to charge up the defib pads?
    #define CHARGE_TIME 3000
    // How long to leave "Shock delivered" displayed after a shock, before allowing it to be overwritten by "Start CPR"?
    #define STOP_TIME 1000


//
// CPR PARAMETERS
//
    // Standard CPR timer: 120 sec
    #define CPR_TIME_COMMON 120000
    #define CPR_TIME_ADULT CPR_TIME_COMMON
    #define CPR_TIME_CHILD CPR_TIME_COMMON

    // CPR compression depth parameters (1/10th inches)
    #define CPR_ADULT_TARGET_DEPTH_MIN 20
    #define CPR_ADULT_TARGET_DEPTH_MAX 24
    #define CPR_DEPTH_SLIDER_MIN 0

    // How long should we wait for compressions, before displaying "CONTINUE CPR" prompt?
    #define CPR_DELAY_TOLERANCE 4000

    // CPM thressholds for giving "go slightly faster" or "go faster" prompts
    #define CPR_SLOW_THRESHHOLD 95
    #define CPR_VERYSLOW_THRESHHOLD 80

    #define CPR_RATE_SLIDER_MIN 40


#ifdef AEDCONST_DEBUG_ENABLE

    #undef  TEST_TIME
    #define TEST_TIME 1000

    #undef  AED_TIMER_DEFAULT
    #define AED_TIMER_DEFAULT 1000

    #undef  ECG_TIME
    #define ECG_TIME 1000

    #undef  CHARGE_TIME
    #define CHARGE_TIME 1000

    #undef  CPR_TIME_COMMON
    #define CPR_TIME_COMMON 6000

#endif // AEDCONST_DEBUG_ENABLE switch

#endif // AEDCONST_H

