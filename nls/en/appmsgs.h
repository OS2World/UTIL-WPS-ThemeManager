// thememgr.h

#ifndef APPMSGS_H_
   #define APPMSGS_H_

// window captions
#define SZ_TITLE              "Frame Control Themes"
#define SZ_PREVIEW            "~Preview: "
#define SZ_THEMES             "Available ~themes "
#define SZ_APPLY              "~Apply"
#define SZ_CANCEL             "~Cancel"
#define SZ_HELP               "Help"


// error messages
#define SZERR_PARSEIDX     "The file THEMEMGR.IDX is missing or invalid!"
#define SZERR_BOOTDRIVE    "Failed to get the boot drive!"
#define SZERR_COPYMODULE   "Failed to replace \\OS2\\DLL\\PMMRGRES.DLL with "
#define SZERR_WRITEINI     "Failed to store the theme name in OS2.INI"
// apply message
#define SZ_SUCCESS         "The selected theme has been successfully applied. "\
                           "Please reboot now to enable it!\x9Please notice:"
// preview window captions:
#define SZ_PREVUNKNOWN     "~Preview (current theme unknown):"

#endif // #ifndef APPMSGS_H_
