#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <pthread.h>
#include <unistd.h>

const UniChar ĉ = 265;
const UniChar ĝ = 285;
const UniChar ĥ = 293;
const UniChar ĵ = 309;
const UniChar ŝ = 349;
const UniChar ŭ = 365;

const CGKeyCode KEY_X =  7;
const CGKeyCode KEY_C =  8;
const CGKeyCode KEY_G =  5;
const CGKeyCode KEY_H =  4;
const CGKeyCode KEY_J = 38;
const CGKeyCode KEY_S =  1;
const CGKeyCode KEY_U = 32;

void simulate_key (UniChar c) {
    CGEventRef downEvt = CGEventCreateKeyboardEvent( NULL, 0, true );
    CGEventRef upEvt   = CGEventCreateKeyboardEvent( NULL, 0, false );
    CGEventKeyboardSetUnicodeString (downEvt, 1, &c);
    CGEventKeyboardSetUnicodeString (upEvt,   1, &c);
    CGEventPost (kCGAnnotatedSessionEventTap, downEvt);
    CGEventPost (kCGAnnotatedSessionEventTap, upEvt);
}

void simulate_delete () {
    CGEventRef downEvt = CGEventCreateKeyboardEvent( NULL, 0, true );
    CGEventRef upEvt   = CGEventCreateKeyboardEvent( NULL, 0, false );
    CGEventSetIntegerValueField (downEvt, kCGKeyboardEventKeycode, (CGKeyCode) 51);
    CGEventSetIntegerValueField (upEvt,   kCGKeyboardEventKeycode, (CGKeyCode) 51);
    CGEventPost (kCGSessionEventTap, downEvt);
    CGEventPost (kCGSessionEventTap, upEvt);
}

// This callback will be invoked every time there is a keystroke.
CGEventRef myCGEventCallback_xc (CGEventTapProxy proxy, CGEventType type, CGEventRef event, void * data) {

    if (type == kCGEventFlagsChanged) {
        // TO-DO: check if X is upper case
        return event;
    }
    else if (type != kCGEventKeyDown)
        return event;
    
    // The incoming keycode
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    
    bool * pressed_x = data;

    if (*pressed_x) {
        *pressed_x = false;

        UniChar key;
        switch (keycode) {
            case KEY_C: key = ĉ; break;
            case KEY_G: key = ĝ; break;
            case KEY_H: key = ĥ; break;
            case KEY_J: key = ĵ; break;
            case KEY_S: key = ŝ; break;
            case KEY_U: key = ŭ; break;
            default:
                return event;
        }
        
        simulate_key (key);
        return 0;
    }

    if (keycode != (CGKeyCode) KEY_X)
        return event;

    *pressed_x = true;
    return 0;
}


void * press_key_thread (void * key) {
    usleep(100);
    simulate_key ((UniChar) key);
    return NULL;
}

// This callback will be invoked every time there is a keystroke.
CGEventRef myCGEventCallback_cx (CGEventTapProxy proxy, CGEventType type, CGEventRef event, void * data) {

    if (type != kCGEventKeyDown)
        return event;

    // Check if only capslock or shift are being pressed
    CGEventFlags flags = CGEventSourceFlagsState (kCGEventSourceStateHIDSystemState);
    bool upper = (kCGEventFlagMaskAlphaShift & flags) ^ (kCGEventFlagMaskShift & flags);
    
    // The incoming keycode
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

    UniChar * last_key = data;

    // Store last key pressed
    switch (keycode) {
        case KEY_C: *last_key = ĉ; break;
        case KEY_G: *last_key = ĝ; break;
        case KEY_H: *last_key = ĥ; break;
        case KEY_J: *last_key = ĵ; break;
        case KEY_S: *last_key = ŝ; break;
        case KEY_U: *last_key = ŭ; break;
        case KEY_X:                break;
        default:    *last_key = 0;
    }

    if (*last_key == 0)
        return event;

    if (keycode != (CGKeyCode) KEY_X) {
        
        // Result diacritic wil be uppercase
        if (*last_key > 0 && upper)
            (*last_key)--;

        return event;   
    }
    
    simulate_delete ();
    
    // Creates thread to send diacritic after a while
    pthread_t tid;
    pthread_create (&tid, NULL, press_key_thread, (void *) (size_t) *last_key);

    return 0;
}

int main (int argc, char ** args) {
    printf ("╔═══════════════════════════════════════════════════════════════════════╗\n"
            "║ iEsperantilo 1.0 is running globally in all windows!                  ║\n"
            "║ Press a letter and x to produce the correspondent diacritics (ŝĉĝĥĵŭ) ║\n"
            "║ Press Ctrl + C in this window to turn off.                            ║\n"
            "║ Source code and infos: https://github.com/G4BB3R/Esperantilo-macOS    ║\n" 
            "╚═══════════════════════════════════════════════════════════════════════╝\n");

    CGEventMask eventMask = (1 << kCGEventKeyDown) ;

    UniChar state = 0;
    CFMachPortRef eventTap =
        CGEventTapCreate (kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask,
            argc > 1 ? myCGEventCallback_xc : myCGEventCallback_cx, &state);

    if (! eventTap) {
        printf ("Failed to hook. Check file permissions, use sudo or enable "
                "access for assistive devices in system settings.\n");
        exit (1);
    }
    
    // Create a run loop source.
    CFRunLoopSourceRef runLoopSource =
        CFMachPortCreateRunLoopSource (kCFAllocatorDefault, eventTap, 0);
    
    // Add to the current run loop.
    CFRunLoopAddSource (CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    
    // Enable the event tap.
    CGEventTapEnable (eventTap, true);
    
    // Set it all running.
    CFRunLoopRun ();
    
    exit (0);
}