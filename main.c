#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

const uint KEY_X_c = 265;
const uint KEY_X_C = 264;
const uint KEY_X_g = 285;
const uint KEY_X_G = 284;
const uint KEY_X_h = 293;
const uint KEY_X_H = 292;
const uint KEY_X_j = 309;
const uint KEY_X_J = 308;
const uint KEY_X_s = 349;
const uint KEY_X_S = 348;
const uint KEY_X_u = 365;
const uint KEY_X_U = 364;
const uint KEY_x   = 7;
const uint KEY_X   = 7;

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

// Global state
bool pressed_x = false; 

// This callback will be invoked every time there is a keystroke.
CGEventRef myCGEventCallback (CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {

    if (type == kCGEventFlagsChanged) {

    }
    else  if (type != kCGEventKeyDown) {
        printf (" WHAAAAAAAAT %d \n", type);
        return event;
    }
    
    // The incoming keycode
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        
    if (pressed_x) {
        pressed_x = false;

        UniChar key;
        switch (keycode) {
            case KEY_C: key = KEY_X_c; break;
            case KEY_G: key = KEY_X_g; break;
            case KEY_H: key = KEY_X_h; break;
            case KEY_J: key = KEY_X_j; break;
            case KEY_S: key = KEY_X_s; break;
            case KEY_U: key = KEY_X_u; break;
            //case KEY_x: key = KEY_x;   break;
            default:
                return event;
        }
        
        simulate_key (key);
        return 0;
    }

    if (keycode != (CGKeyCode) KEY_X)
        return event;

    pressed_x = true;
    return 0;
}

int main () {
    
    printf ("╔═══════════════════════════════════════════════════════════════════════╗\n"
            "║ iEsperantilo 1.0 is running globally in all windows!                  ║\n"
            "║ Press x and a letter to produce the correspondent diacritics (ŝĉĝĥĵŭ) ║\n"
            "║ Press Ctrl + C in this window to turn off.                            ║\n"
            "╚═══════════════════════════════════════════════════════════════════════╝\n");

    CGEventMask eventMask = (1 << kCGEventKeyDown)
                          | (1 << kCGEventFlagsChanged) ;

    CFMachPortRef eventTap  = CGEventTapCreate (kCGSessionEventTap,
        kCGHeadInsertEventTap, 0, eventMask, myCGEventCallback, NULL);

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