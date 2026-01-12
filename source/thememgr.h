// thememgr.h

#ifndef _THEMEMGR_H_
   #define _THEMEMGR_H_

#define INCL_GPI
#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <winutil.h>
#include "appid.h"
//#include "appmsgs.h"

// flags usate da Wprint

#define PMPRNT_WININIT     0x0001    // richiede inizializzazione
#define PMPRNT_ERROR       0x0000    // Errore
#define PMPRNT_WARN        0x0002    // Attenzione
#define PMPRNT_INFO        0x0004    // Nota
#define PMPRNT_QUERY       0x0008    // richiesta conferma
#define PMPRNT_SYSMOD      0x0010    // system modal
// flags valide in modalit… PMPRNT_QUERY
#define PMPRNT_QWARNICO    0x0020
#define PMPRNT_QINFOICO    0x0040
#define PMPRNT_QERRORICO   0x0080
#define PMPRNT_QQUERYICO   0x0000

APIRET APIENTRY DosReplaceModule(PSZ pszOld, PSZ pszNew, PSZ pszBkup);


// dati singolo tema
typedef struct {
   CHAR achName[256];         // descrizione tema
   CHAR achRes[256];          // nome file risorse
   CHAR achBmp[256];          // nome file bitmap preview
} THEME, * PTHEME;

#pragma pack(2)
// Lista temi
typedef struct {
   USHORT cthms;              // numero di temi nella lista
   SHORT icthm;               // indice tema corrente
   CHAR achCurTheme[256];     // nome tema corrente letto da OS2.INI
   THEME athms[1];            // lista temi
} THEMELIST, * PTHEMELIST;
#pragma pack()

#define SZFILE_HLP            "THEMEMGR.HLP"

#define SZPRO_APP             "ThemeManager"
#define SZPRO_KEY             "CurrentTheme"

#define SZ_THEMEIDX           "THEMEMGR.IDX"
#define THMTAG                   ""
#define INVTAG                ((PSZ)-1)
#define CXPREVIEW             240
#define CYPREVIEW             180
#define SZ_MODULENAME         "D:\\OS2\\DLL\\PMMRGRES.DLL"

#define getSval(x)        (WinQuerySysValue(HWND_DESKTOP, (x)))

// prototipi funzioni
// dllapi.c
VOID msgbox(HWND hwnd, PSZ pszMsg, ULONG fl);
PSZ ioF2psz(PSZ pszFile, PULONG pcb);
BOOL WsetDlgIcon(HWND hwnd, HMODULE hmod, ULONG id);
LONG loadStr(HWND hwnd, ULONG idstr, PSZ psz);
VOID WsetCtlTxt(HWND hwnd, ULONG idctl, ULONG idstr, PSZ pappend);
// thememgr.c
// util.c
PTHEMELIST parseThemeMgrIdx(PSZ pszFile);
VOID placeControls(HWND hwnd, PSIZEL pszl);
HBITMAP loadBmpFile(HPS hps, PSZ pfile);
INT findTheme(PTHEMELIST ptl, PSZ pszTheme);
BOOL applyTheme(HWND hwnd, PTHEMELIST ptl);
HWND initHelp(HWND hwnd);
VOID endHelp(HWND hwnd, HWND hHlp);

#endif // #ifndef _THEMEMGR_H_
