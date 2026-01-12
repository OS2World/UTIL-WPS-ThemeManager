/* --------------------------------------------------------------------------
 This script builds multiple language versions of
-------------------------------------------------------------------------- */

/* initialization -------------------------------------------------------- */
"@echo off"
"cls"
call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs"
call SysLoadFuncs

appname = "Theme Manager"
basename = "THEMEMGR"
languages = "Dutch English French German Italian Russian Spanish"
langpath     = "NL  EN  FR  DE  IT  RU  ES";
countrycodes = "031 001 033 049 039 007 034";
codepages    = "850 850 850 850 850 866 050";
ipflang      = "NLD ENU FRC DEU ITA RUS ESP";
/* select the national language version */
selection = selLanguage();
if selection = 0 then exit
/* check if the executable is up to date via NMAKE */
call buildExe selection;
/* build the resources */
call buildRes selection;
"pause"
exit

/* language selection prompt --------------------------------------------- */
selLanguage: procedure expose appname
   say sourceline(2)" "appname".";
   say
   say " Select a language:"
   say " (1) Dutch"
   say " (2) English"
   say " (3) French"
   say " (4) German"
   say " (5) Italian"
   say " (6) Russian"
   say " (7) Spanish"
   say " (0) Terminate the program"
   /* call SysCurPos 0, 0; */
   do forever
      selection = SysGetKey("NOECHO");
      if (selection < 0) | (selection > 7) then
         call Beep 440, 300;
      else
         leave
   end /* do */
   return selection


/* checking if THEMEMGR.EXE is up to date via NMAKE ---------------------- */
buildExe: procedure expose languages appname
parse arg selection
   "cls"
   say " Building the "word(languages, selection)" version of "appname"..."
   "pause"
   call directory source;
   "cls"
   do forever
      "nmake -nologo"
      if RC = 0 then leave
      say "Press any key to restart NMAKE."
      say "Press Ctrl-C to stop the building process."
      "pause > nul"
   end /* do */
   return


/* executing RC.EXE to bind the resources to the executable -------------- */
buildRes: procedure expose langpath countrycodes codepages ipflang basename
parse arg selection
   say " "copies("-------", 11);
   say " compiling the application resources..."
   "pause"
   "copy "basename".exe ..\nls\"word(langpath, selection)
   call directory "..\nls\"word(langpath, selection);
   "mode co80,50"
   cc = word(countrycodes, selection);
   cp = word(codepages, selection);
   includes = "-i ..\..\source"
   iconfile = directory()"\..\..\res\"basename".ico"
   bmpfile = directory()"\"basename".bmp"
   defines = "-d SZ_APPICON="iconfile" -d SZ_APPBMP="bmpfile
   rcfile = "..\..\res\"basename".rc"
   "rc -n -r -cc "cc" -cp "cp" "includes" "defines" "rcfile" "basename".res"
   if RC \= 0 then return
   say " "copies("-------", 11);
   say " binding the resources to the executable..."
   "pause"
   "cls"
   "rc -x2 "basename".res "basename".exe"
   if RC \= 0 then return
   say " "copies("-------", 11);
   say " building the help file..."
   "pause"
   "cls"
   il = word(ipflang, selection);
   "IPFC -s -x -D:"cc" -C:"cp" -L:"il" "basename".ipf"
   say " "copies("-------", 11);
   if RC = 0 then
      say "The build process terminated succesfully"
   return
