//
//  AppDelegate.swift
//  Esperantilo
//
//  Created by Gabriel Torrecillas Sartori on 19/04/18.
//  Copyright © 2018 Gabriel Torrecillas Sartori. All rights reserved.
//

import Cocoa
import Security
import Foundation

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    
    let widget = NSStatusBar.system.statusItem(withLength: -1)
    let icon   = NSImage(named: NSImage.Name("icon"))
    
    @IBOutlet weak var menu: NSMenu!
    @IBOutlet weak var btnEnable: NSMenuItem!
    
    var failed = false;
    
    @IBAction func enableClicked(_ sender: Any) {
        if !failed {
            let disable = btnEnable.title == "Enable"
            btnEnable.title = disable ? "Disable" : "Enable"
            widget.image = disable ? icon : nil
            core_enable (disable ? 1 : 0)
        }
    }
    
    @IBAction func btnInfo(_ sender: Any) {
        NSWorkspace.shared.open(URL(string: "https://github.com/G4BB3R/Esperantilo-macOS")!)
    }
    
    @IBAction func quitClicked(_ sender: NSMenuItem) {
        NSApplication.shared.terminate(self)
    }
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        widget.image = icon
        
        widget.title = "Eo"
        widget.menu = menu
        
        if core_create_event_tap() == 0 {
            self.failed = true;
            btnEnable.title = "Failed! No permissions :(";
            
            let options : NSDictionary = [kAXTrustedCheckOptionPrompt.takeRetainedValue() as NSString: true]
            let _ = AXIsProcessTrustedWithOptions(options) // This line prompts with a window for accesibility
        }
    }

}

