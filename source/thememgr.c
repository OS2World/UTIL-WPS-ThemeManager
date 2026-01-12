// thememgr.c
// INT main(VOID);
// MRESULT EXPENTRY mainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// VOID initDlg(HWND hwnd);
// VOID ctrlDlg(HWND hwnd, ULONG id, ULONG ulevnt, HWND hctrl);
// VOID cmdDlg(HWND hwnd, ULONG id);
// MRESULT EXPENTRY newImgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#include "thememgr.h"


// prototipi funzioni
MRESULT EXPENTRY mainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID initDlg(HWND hwnd);
VOID ctrlDlg(HWND hwnd, ULONG id, ULONG ulevnt, HWND hctrl);
VOID cmdDlg(HWND hwnd, ULONG id);
MRESULT EXPENTRY newImgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


static PTHEMELIST ptl;            // lista temi disponibili
static PFNWP oldImgProc;          // procedura default WC_STATIC
static HBITMAP hbmp;              // handle immagine di preview

INT main(int argc, char** argv) {
   HAB hab;
   HMQ hmq;
   QMSG qmsg;
   HWND hwnd;
   HWND hHlp;
   // legge da file thememgr.idx lista temi disponibili
   if (!(ptl = parseThemeMgrIdx(argc > 1? argv[1]: NULL))) {
      msgbox(HWND_DESKTOP, (PSZ)IDERR_PARSEIDX, TRUE);
      return 10;
   } /* endif */
   // inizializzazione PM
   hmq = WinCreateMsgQueue((hab = WinInitialize(0)), 0);
   // lettura OS2.INI (trova tema correntemente installato)
   if (!PrfQueryProfileString(HINI_USER, SZPRO_APP, SZPRO_KEY, "",
                              ptl->achCurTheme, 256) ||
       !ptl->achCurTheme[0]) {
      ptl->icthm = -1;
   } else {
      ptl->icthm = findTheme(ptl, ptl->achCurTheme);
   } /* endif */
   // caricamento dialogo principale
   hwnd = WinLoadDlg(HWND_DESKTOP, NULLHANDLE, mainDlgProc, NULLHANDLE,
                     DLG_MAIN, NULL);
   if (hwnd) {
      hHlp = initHelp(hwnd);
      while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
         WinDispatchMsg(hab, &qmsg);
      endHelp(hwnd, hHlp);
   } /* endif */
   // liberazione risorse e chiusura applicazione
   GpiDeleteBitmap(hbmp);
   free(ptl);
   WinDestroyWindow(hwnd);
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
} /* end of main */


MRESULT EXPENTRY mainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      case WM_INITDLG:
         initDlg(hwnd);
         break;
      case WM_CONTROL:
         ctrlDlg(hwnd, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1), (HWND)mp2);
         break;
      case WM_COMMAND:
         cmdDlg(hwnd, (ULONG)mp1);
         break;
      case WM_CLOSE:
         WinPostMsg(hwnd, WM_QUIT, MPVOID, MPVOID);
         break;
//      case WM_WINDOWPOSCHANGED:
//         return formatDlg(hwnd, mp1, mp2);
//      case WM_SETBORDERSIZE:
//         return borderDlg(hwnd, mp1, mp2);
//      case WM_MINMAXFRAME:
//         return minmaxDlg(hwnd, mp1, mp2);
//      case WM_QUERYTRACKINFO:
//         return trackDlg(hwnd, mp1, mp2);
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   } /* endswitch */
   return MRFALSE;
}


// inizializzazione dialogo
VOID initDlg(HWND hwnd) {
   ULONG cxscr = getSval(SV_CXSCREEN);
   ULONG cyscr = getSval(SV_CYSCREEN);
   SIZEL szl;
   INT i;
   HWND hlist = WinWindowFromID(hwnd, LBX_MAIN);
   // posiziona controlli e calcola dimensioni dialogo
   placeControls(hwnd, &szl);
   // subclassa controllo statico
   oldImgProc = WinSubclassWindow(WinWindowFromID(hwnd, IMG_MAIN), newImgProc);
   // riempie lista temi
   for (i = 0; i < ptl->cthms; ++i)
      wLboxInsItem(hlist, LIT_END, ptl->athms[i].achName);
   // seleziona tema precedente
   if (ptl->icthm >= 0) {
      wLboxSelItem(hlist, ptl->icthm);
   } else {
      WsetCtlTxt(hwnd, GRP_PRVIEW, ID_PREVUNKNOWN, NULL);  
   } /* endif */
   // setta icona dialogo
   WsetDlgIcon(hwnd, 0, 1);
   // centra il dialogo sullo schermo
   WinSetWindowPos(hwnd, HWND_TOP,
                   (cxscr - szl.cx) / 2, (cyscr - szl.cy) / 2, szl.cx, szl.cy,
                   SWP_SIZE | SWP_MOVE | SWP_SHOW | SWP_ZORDER);
}


// alla selezione di un nuovo item libera vecchio hbmp e carica nuovo
VOID ctrlDlg(HWND hwnd, ULONG id, ULONG ulevnt, HWND hctrl) {
   if (id == LBX_MAIN && ulevnt == LN_SELECT) {
      ptl->icthm = wLboxQuerySelItem(hctrl);
      if (ptl->icthm >= 0) {
         HPS hps = WinGetPS(hwnd);
         GpiDeleteBitmap(hbmp);
         hbmp = loadBmpFile(hps, ptl->athms[ptl->icthm].achBmp);
         WinReleasePS(hps);
         WinInvalidateRect(WinWindowFromID(hwnd, IMG_MAIN), NULL, FALSE);
         WsetCtlTxt(hwnd, GRP_PRVIEW, ID_PREVIEW,
                    ptl->athms[ptl->icthm].achName);
         WinEnableControl(hwnd, BTN_APPLY, TRUE);
      } /* endif */
   } /* endif */
}


// sostituisce il modulo corrente con quello corrispondente al tema selezionato
VOID cmdDlg(HWND hwnd, ULONG id) {
   switch (id) {
      case BTN_APPLY:
         if (!applyTheme(hwnd, ptl)) break;
      case DID_CANCEL:
      case BTN_CANCEL:
         WinPostMsg(hwnd, WM_QUIT, MPVOID, MPVOID);
         break;
   } /* endswitch */
}


// procedura subclassata immagine statica

MRESULT EXPENTRY newImgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   if (msg == WM_PAINT && hbmp) {
      MRESULT mr = oldImgProc(hwnd, msg, mp1, mp2);
      HPS hps = WinGetPS(hwnd);
      RECTL r = {52, 0, 106, 18};
      POINTL pt = {177, 148};
      // disegna controlli close/hide/restore
      WinDrawBitmap(hps, hbmp, &r, &pt, 0, 0, DBM_NORMAL);
      // disegna controlli close/min/max
      WRectSet(&r, 106, 0, 160, 18);
      WPointSet(&pt, 166, 75);
      WinDrawBitmap(hps, hbmp, &r, &pt, 0, 0, DBM_NORMAL);
      // disegna radiobutton unchecked
      WRectSet(&r, 0, 0, 13, 13);
      WPointSet(&pt, 6, 26);
      WinDrawBitmap(hps, hbmp, &r, &pt, 0, 0, DBM_NORMAL);
      // disegna radiobutton checked
      WRectSet(&r, 13, 0, 26, 13);
      WPointSet(&pt, 6, 6);
      WinDrawBitmap(hps, hbmp, &r, &pt, 0, 0, DBM_NORMAL);
      // disegna checkbox unchecked
      WRectSet(&r, 26, 0, 39, 13);
      WPointSet(&pt, 95, 26);
      WinDrawBitmap(hps, hbmp, &r, &pt, 0, 0, DBM_NORMAL);
      // disegna checkbox checked
      WRectSet(&r, 39, 0, 52, 13);
      WPointSet(&pt, 95, 6);
      WinDrawBitmap(hps, hbmp, &r, &pt, 0, 0, DBM_NORMAL);
      WinReleasePS(hps);
   } /* endif */
   return oldImgProc(hwnd, msg, mp1, mp2);
}
