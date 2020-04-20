#include "tray_ui.h"
#include <services/service_manager.h>
#include <utility/time_utils.h>
#import <objc/objc.h>
#include <objc/objc-runtime.h>
#include <climits>
#import <cstring>

bool TrayUI::shouldQuit = false;

static id app;
static id pool;
static id statusBar;
static id statusItem;
static id statusBarButton;

void StartSystemCallback(struct tray_menu *item) {
    SM::getDeviceManager()->start();
}

void StopSystemCallback(struct tray_menu *item) {
    SM::getDeviceManager()->stop();
}

void QuitCallback(struct tray_menu *item) {
    TrayUI::shouldQuit = true;
}

static void menu_callback(id self, SEL cmd, id sender) {
    id asdf = ((id(*)(id, SEL))objc_msgSend)(sender, sel_registerName("representedObject"));
    struct tray_menu *m = ((struct tray_menu*(*)(id, SEL))objc_msgSend)(asdf,sel_registerName("pointerValue"));
    if (m != NULL && m->cb != NULL) {
        m->cb(m);
    }
}

static id _tray_menu(struct tray_menu *m) {
    id menu = ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSMenu"), sel_registerName("new"));
    ((void(*)(id, SEL))objc_msgSend)(menu, sel_registerName("autorelease"));
    ((void(*)(id, SEL, bool))objc_msgSend)(menu, sel_registerName("setAutoenablesItems:"), false);

    for (; m != NULL && m->text != NULL; m++) {
        if (strcmp(m->text, "-") == 0) {
            ((void(*)(id, SEL, id))objc_msgSend)(menu, sel_registerName("addItem:"),
                                                 ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSMenuItem"), sel_registerName("separatorItem")));
        } else {
            id menuItem = ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
            ((void(*)(id, SEL))objc_msgSend)(menuItem, sel_registerName("autorelease"));
            ((void(*)(id, SEL, id, SEL, id))objc_msgSend)(menuItem, sel_registerName("initWithTitle:action:keyEquivalent:"),
                                                          ((id(*)(id, SEL, char *))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), m->text),
                                                          sel_registerName("menuCallback:"),
                                                          ((id(*)(id, SEL, char *))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), ""));

            ((void(*)(id, SEL, bool))objc_msgSend)(menuItem, sel_registerName("setEnabled:"), m->disabled == 0);
            ((void(*)(id, SEL, int))objc_msgSend)(menuItem, sel_registerName("setState:"), (m->checked ? 1 : 0));
            ((void(*)(id, SEL, id))objc_msgSend)(menuItem, sel_registerName("setRepresentedObject:"),
                                                 ((id(*)(id, SEL, struct tray_menu*))objc_msgSend)((id)objc_getClass("NSValue"), sel_registerName("valueWithPointer:"), m));

            ((void(*)(id, SEL, id))objc_msgSend)(menu, sel_registerName("addItem:"), menuItem);

            if (m->submenu != NULL) {
                ((void(*)(id, SEL, id, id))objc_msgSend)(menu, sel_registerName("setSubmenu:forItem:"), _tray_menu(m->submenu), menuItem);
            }
        }
    }

    return menu;
}

void TrayUI::init() {
    SM::getLogger()->info("Started Tray UI.");

    pool = ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSAutoreleasePool"), sel_registerName("new"));
    ((void(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));

    Class trayDelegateClass = objc_allocateClassPair(objc_getClass("NSObject"), "Tray", 0);
    class_addProtocol(trayDelegateClass, objc_getProtocol("NSApplicationDelegate"));
    class_addMethod(trayDelegateClass, sel_registerName("menuCallback:"), (IMP)menu_callback, "v@:@");
    objc_registerClassPair(trayDelegateClass);

    id trayDelegate = ((id(*)(id, SEL))objc_msgSend)((id)trayDelegateClass, sel_registerName("new"));

    app = ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));

    ((void(*)(id, SEL,id))objc_msgSend)(app, sel_registerName("setDelegate:"), trayDelegate);
    statusBar = ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSStatusBar"), sel_registerName("systemStatusBar"));
    statusItem = ((id(*)(id, SEL, double))objc_msgSend)(statusBar, sel_registerName("statusItemWithLength:"), -1.0);

    ((void(*)(id, SEL))objc_msgSend)(statusItem, sel_registerName("retain"));
    ((void(*)(id, SEL, bool))objc_msgSend)(statusItem, sel_registerName("setHighlightMode:"), true);
    statusBarButton = ((id(*)(id, SEL))objc_msgSend)(statusItem, sel_registerName("button"));
    TrayUI::updateMenu();
    ((void(*)(id, SEL, bool))objc_msgSend)(app, sel_registerName("activateIgnoringOtherApps:"), true);
}

struct tray trayDefinition = {
        .icon = "icon2.png",
        .menu = (struct tray_menu[]){{"TelemetryJet Server v0.1.0", 1, 0, NULL, NULL},
                                     {"-", 0, 0, NULL, NULL},
                                     {"Quit", 0, 0, QuitCallback, NULL},
                                     {NULL, 0, 0, NULL, NULL}},
};

void TrayUI::updateMenu() {
    ((void(*)(id, SEL, id))objc_msgSend)(statusBarButton, sel_registerName("setImage:"),
    ((id(*)(id, SEL, id))objc_msgSend)((id)objc_getClass("NSImage"), sel_registerName("imageNamed:"),
    ((id(*)(id, SEL, char *))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), trayDefinition.icon)));

    ((void(*)(id, SEL, id))objc_msgSend)(statusItem, sel_registerName("setMenu:"), _tray_menu(trayDefinition.menu));
}


void TrayUI::update() {
    id until = ((id(*)(id, SEL))objc_msgSend)((id)objc_getClass("NSDate"), sel_registerName("distantPast"));
    id event = ((id(*)(id, SEL, unsigned long, id, id, bool))objc_msgSend)(app, sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"),
                                                                           ULONG_MAX,
                                                                           until,
                                                                           ((id(*)(id, SEL, char*))objc_msgSend)((id)objc_getClass("NSString"),
                                                                                                                 sel_registerName("stringWithUTF8String:"),
                                                                                                                 "kCFRunLoopDefaultMode"),
                                                                           true);
    if (event) {
        ((void(*)(id, SEL, id))objc_msgSend)(app, sel_registerName("sendEvent:"), event);
    }
}

void TrayUI::destroy() {
    SM::getLogger()->info("Stopped Tray UI.");
}