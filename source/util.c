// util.c
// PSZ getToken(PSZ p, PSZ pendl, PSZ * ppnext);
// PTHEMELIST parseThemeMgrIdx(VOID);
// VOID placeControls(HWND hwnd, PSIZEL pszl);
// HBITMAP loadBmpFile(HPS hps, PSZ pfile);

#include "thememgr.h"

// prototipi funzioni
PSZ getToken(PSZ p, PSZ pendl, PSZ * ppnext);


PSZ getToken(PSZ p, PSZ pendl, PSZ * ppnext) {
   PSZ pnext;
   // this is not executed for the last token in the line (image file name)
   if (ppnext) {
      // if the next tag is missing exits the loop
      if (!(pnext = strstr(p, THMTAG))) return NULL;
      // if this line is not complete skips to the next line
      if (pendl && pnext > pnext) return INVTAG;
      // stores the address of the next token
      *ppnext = pnext + 3;
   } else {
      pnext = pendl;
   } /* endif */
   // strips leading spaces and control chars
   while (*p <= ' ' && p < pendl) p++;
   // if it is an empty description checks the next line
   if (ppnext && p == pendl) return INVTAG;
   // strips trailing spaces and terminates the token with a \x0
   while (*pnext <= ' ') --pnext;
   *(pnext + 1) = 0;
   return p;
}


PTHEMELIST parseThemeMgrIdx(PSZ pszFile) {
   PTHEMELIST ptl = NULL;
   PSZ psz, p, pdesc, pres, pimg, pendl;
   ULONG ul = 1;
   CHAR achbuf[260];
   // checks if it must change path
   if (pszFile &&
       (DosQueryPathInfo(pszFile, FIL_QUERYFULLNAME, achbuf, 260) ||
        DosSetDefaultDisk(achbuf[0] - 'A' + 1) ||
        DosSetCurrentDir(achbuf)))
      return NULL;
   // reads index file
   if (!(psz = ioF2psz(SZ_THEMEIDX, &ul))) return NULL;
   /// counts amount of theme descriptions
   for (p = psz, ul = 0; (p > (PSZ)1) && *p; p = strpbrk(p, "\r\n") + 1)
      if (!memcmp(p, THMTAG, 3)) ++ul;
   // if no valid theme description has been found returns NULL
   if (!ul) goto end0;
   // allocates storage for theme list
   if (!(ptl = malloc(sizeof(THEMELIST) + sizeof(THEME) * (ul - 1))))
      goto end0;
   // parses the index file
   for (p = psz, ul = 0; p && *p; (p = pendl? pendl + 1: NULL)) {
      pendl = strpbrk(p, "\r\n");
      // if the lines starts with the tag it is parsed
      if (!memcmp(p, THMTAG, 3)) {
         // ----- finds the theme description -----
         if (!(pdesc = getToken(p + 3, pendl, &pres))) break;
         else if (pdesc == INVTAG) continue;
         // ----- finds the theme resource file name -----
         if (!(pres = getToken(pres, pendl, &pimg))) break;
         else if (pres == INVTAG) continue;
         // ----- finds the preview image file name -----
         if (!(pimg = getToken(pimg, pendl, NULL))) break;
         else if (pimg == INVTAG) continue;
         strcpy(ptl->athms[ul].achName, pdesc);
         strcpy(ptl->athms[ul].achRes, pres);
         strcpy(ptl->athms[ul].achBmp, pimg);
         ul++;
      } /* endif */
   } /* endfor */
   if (!ul) goto end1; // if no valid line has been found returns NULL
   ptl->cthms = ul;
   goto end0;
end1:
   free(ptl);
   ptl = NULL;
end0:
   free(psz);
   return ptl;
}


// posiziona i controlli
VOID placeControls(HWND hwnd, PSIZEL pszl) {
   ULONG aid[] = {GRP_PRVIEW, IMG_MAIN, GRP_LIST, LBX_MAIN,
                  BTN_APPLY, BTN_CANCEL, BTN_HELP};
   SWP aswp[7];
   INT i;
   ULONG cxbrd = getSval(SV_CXSIZEBORDER);
   ULONG cybrd = getSval(SV_CYSIZEBORDER);
   ULONG cytitle = getSval(SV_CYTITLEBAR);
   POINTL aptl[] = {{2, 2}, {8, 8}, {50, 13}};
   memset(aswp, 0, sizeof(aswp));
   WinMapDlgPoints(hwnd, aptl, 3, TRUE);
   for (i = 0; i < 7; ++i) {
      aswp[i].hwnd = WinWindowFromID(hwnd, GRP_PRVIEW + i);
      aswp[i].fl = SWP_SIZE | SWP_MOVE | SWP_SHOW;
   } /* endfor */
   // coordinate groupbox lista temi
   aswp[2].x = cxbrd + aptl[0].x;
   aswp[2].y = cybrd + aptl[0].y;
   aswp[2].cx = aptl[0].x * 3 + CXPREVIEW + 4 + aptl[2].x;
   aswp[2].cy = 10 + aptl[0].y * 3 + 8 * aptl[1].y;
   // coordinate listbox lista temi
   aswp[3].x = aswp[2].x + aptl[0].x + 2;
   aswp[3].y = aswp[2].y + aptl[0].y + 2;
   aswp[3].cx = aswp[2].cx - aptl[0].x - aptl[0].x - 4;
   aswp[3].cy = aptl[1].y * 7 + 6;
   // coordinate groupbox immagine preview
   aswp[0].x = aswp[2].x;
   aswp[0].y = aswp[2].y + aswp[2].cy + aptl[0].y;
   aswp[0].cx = 4 + CXPREVIEW + aptl[0].x + aptl[0].x;
   aswp[0].cy = 4 + 3 * aptl[0].y + CYPREVIEW + aptl[1].y;
   // coordinate immagine
   aswp[1].x = aswp[0].x + aptl[0].x + 2;
   aswp[1].y = aswp[0].y + aptl[0].y + 2;
   aswp[1].cx = CXPREVIEW;
   aswp[1].cy = CYPREVIEW;
   // coordinate bottoni
   aswp[4].x = aswp[5].x = aswp[6].x = aswp[0].x + aswp[0].cx + aptl[0].x;
   aswp[4].y = aswp[0].y + aswp[0].cy - aptl[2].y - 2;
   aswp[5].y = aswp[4].y - aptl[2].y - 3;
   aswp[6].y = aswp[5].y - aptl[2].y - 3;
   aswp[4].cx = aswp[5].cx = aswp[6].cx = aptl[2].x;
   aswp[4].cy = aswp[5].cy = aswp[6].cy = aptl[2].y;
   // dimensioni dialogo
   pszl->cx = aswp[2].x + aswp[2].cx + cxbrd + aptl[0].x;
   pszl->cy = aswp[0].y + aswp[0].cy + cybrd + aptl[0].y + cytitle;
   WinSetMultWindowPos(WinQueryAnchorBlock(hwnd), aswp, 7);
}


// legge il file di bitmap, controlla che sia valido e utilizza i dati
// per inizializzare header e dati bitmap
// PBMPHDR indirizzo memorizzazione header descrizione bitmap
// PBYTE pb dati bitmap
// ULONG cbmax massima dimensione ammessa per la bitmap
HBITMAP loadBmpFile(HPS hps, PSZ pfile) {
   HBITMAP hbmp = NULLHANDLE;
   ULONG cb;
   PBYTE pd;
   PBITMAPFILEHEADER2 pbf2;
   cb = 1;
   if (!(pd = ioF2psz(pfile, &cb))) return FALSE;
   // controlla se Š un array di bitmap
   pbf2 = (*((PUSHORT)pd) == BFT_BITMAPARRAY)?
                 &((PBITMAPARRAYFILEHEADER2)pd)->bfh2: (PBITMAPFILEHEADER2)pd;
   // controlla che sia bitmap
   if (pbf2->usType == BFT_BMAP) {
      hbmp =  GpiCreateBitmap(hps, &pbf2->bmp2, CBM_INIT,
                              (PBYTE)pbf2 + pbf2->offBits,
                              (PBITMAPINFO2)&pbf2->bmp2);
   } /* endif */
   free(pd);
   return hbmp;
}


// trova l'indice corrispondente a pszTheme, se questo non Š presente
// restituisce -1
INT findTheme(PTHEMELIST ptl, PSZ pszTheme) {
   INT i;
   for (i = 0; i < ptl->cthms; ++i)
      if (!strcmp(ptl->athms[i].achName, pszTheme)) return i;
   return -1;
}


// applica il tema correntemente selezionato
BOOL applyTheme(HWND hwnd, PTHEMELIST ptl) {
   ULONG ul;
   CHAR buf[512];
   // queries the boot drive
   if (DosQuerySysInfo(QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &ul, 4)) {
      msgbox(hwnd, (PSZ)IDERR_BOOTDRIVE, 0);
      return FALSE;
   } /* endif */
   // unlocks PMMRGRES.DLL
   strcpy(buf, SZ_MODULENAME);
   buf[0] = ul + 'A' - 1;
   DosReplaceModule(buf, NULL, NULL);
   // copies the new DLL over the old one
   if (DosCopy(ptl->athms[ptl->icthm].achRes, buf, DCPY_EXISTING)) {
      if (loadStr(hwnd, IDERR_COPYMODULE, buf)) {
         strcat(buf, ptl->athms[ptl->icthm].achRes);
      } else {
         buf[0] = 0;
      } /* endif */
      msgbox(hwnd, buf, 0);
      return FALSE;
   } /* endif */
   // stores the theme name in OS2.INI
   if (!PrfWriteProfileString(HINI_USER, SZPRO_APP, SZPRO_KEY,
                              ptl->athms[ptl->icthm].achName))
      msgbox(hwnd, (PSZ)IDERR_WRITEINI, 0);
   if (loadStr(hwnd, ID_SUCCESS, buf)) {
      PSZ ptab = strchr(buf, '\t');
      if (ptab) *ptab++ = 0;
      else ptab = "";
      WinMessageBox(HWND_DESKTOP, hwnd, buf, ptab, 0,
                    MB_MOVEABLE | MB_INFORMATION);
   } /* endif */
   return TRUE;
}


// inizializza l'help

HWND initHelp(HWND hwnd) {
   HWND hHlp;
   CHAR buf[256];
   HELPINIT hi;
   loadStr(hwnd, ID_HLPTITLE, buf);
   memset(&hi, 0, sizeof(HELPINIT));
   hi.cb = sizeof(HELPINIT);
   hi.phtHelpTable = PHLPTBLFROMID(HLP_MAIN);
   hi.pszHelpWindowTitle = buf;
   hi.fShowPanelId = CMIC_HIDE_PANEL_ID;
   hi.pszHelpLibraryName = SZFILE_HLP;
   if (!(hHlp = WinCreateHelpInstance(WinQueryAnchorBlock(hwnd), &hi)))
      return NULLHANDLE;
   if (hi.ulReturnCode) {
      WinDestroyHelpInstance(hHlp);
      hHlp = NULLHANDLE;
   } else {
      WinAssociateHelpInstance(hHlp, hwnd);
   } /* endif */
   return hHlp;
}

// termina help

VOID endHelp(HWND hwnd, HWND hHlp) {
   if (hHlp) {
      WinAssociateHelpInstance(NULLHANDLE, hwnd);
      WinDestroyHelpInstance(hHlp);
   } /* endif */
}
