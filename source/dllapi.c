// DLLAPI.c
// BOOL Wprint(HWND hwnd, PSZ pszMsg, ULONG fl);
// PSZ ioF2psz(PSZ pszFile, PULONG pcb);

#include "thememgr.h"


//==========================================================================\
// mostra una message box                                                   |
//==========================================================================/

VOID msgbox(HWND hwnd, PSZ pszMsg, ULONG fl) {
   CHAR buf[256];
   HAB hab = NULLHANDLE;
   HMQ hmq = NULLHANDLE;
   if (fl) hmq = WinCreateMsgQueue((hab = WinInitialize(0)), 0);
   // se Š id stringa la carica dalle risorse
   if ((ULONG)pszMsg < 0xffff) {
      if (!loadStr(hwnd, (ULONG)pszMsg, buf)) buf[0] = 0;
      pszMsg = buf;
   } /* endif */
   WinMessageBox(HWND_DESKTOP, hwnd, pszMsg, NULL, 0, MB_MOVEABLE | MB_ERROR);
   if (fl) {
      WinDestroyMsgQueue(hmq);
      WinTerminate(hab);
   } /* endif */
}


//==========================================================================\
// alloca la memoria necessaria e vi scrive il contenuto del file           |
// nell'ULONG puntato da pcb scrive la dimensione del file letto            |
// restituendo un puntatore all'area di memoria allocata o NULL in caso     |
// di errore                                                                |
// parametri:                                                               |
// PSZ pszFile: nome del file da leggere                                    |
// PULONG pcb : (in) alloca per la lettura (e successiva editazione) una    |
//              quantit… di caratteri pari alla dimensione del file per     |
//              *pcb. (out) quantit… caratteri letti                        |
//              se pcb == 0 si limita a restituire la dimensione del file   |
//              senza leggerlo                                              |
// valore restituito:                                                       |
// PSZ : indirizzo area memoria in cui Š stato scritto il contenuto del file|
//==========================================================================/

PSZ ioF2psz(PSZ pszFile, PULONG pcb) {
   FILESTATUS3 fs;
   HFILE hf;
   ULONG ul;
   PSZ psz;

   if (DosOpen(pszFile,  &hf, &ul, 0, FILE_NORMAL,
               OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
               OPEN_FLAGS_SEQUENTIAL | OPEN_SHARE_DENYWRITE |
               OPEN_ACCESS_READONLY, NULL))
      goto error0;
   if (DosQueryFileInfo(hf, FIL_STANDARD, &fs, sizeof(fs))) goto error1;
   // alloca la dimensione del file oppure moltiplica questa per *pcb
   ul = (pcb && *pcb)? *pcb: 1;
   if (!pcb || (pcb && *pcb)) {                // legge il file
      if (!(psz = (PSZ)malloc(ul * fs.cbFile + 1))) goto error1;
      if (DosRead(hf, psz, fs.cbFile, &ul)) goto error2;
   } /* endif */

   if (DosClose(hf)) goto error2;

   if (pcb) *pcb = fs.cbFile;
   *(psz + fs.cbFile) = 0;     // termina file
   return psz;

   error2: free(psz);
   error1: DosClose(hf);
   error0: return NULL;
}


//==========================================================================\
// Imposta l'icona di una finestra di dialogo caricando la risorsa id dal   |
// modulo hmod. Se id = 0 considera hmod come l'indirizzo della stringa     |
// del nome del file di icona                                               |
// parametri:                                                               |
// HWND hwnd    : handle della dialog window                                |
// PVOID hmod   : indirizzo handle modulo contente icona o nome file icona  |
// ULONG id     : id risorsa                                                |
// valore restituito:                                                       |
// BOOL: TRUE/FALSE = successo/errore                                       |
//==========================================================================/

BOOL WsetDlgIcon(HWND hwnd, HMODULE hmod, ULONG id) {
   HPOINTER hptr = WinLoadPointer(HWND_DESKTOP, hmod, id);
   BOOL rc = (BOOL)WinSendMsg(hwnd, WM_SETICON, (MPARAM)hptr, MPVOID);
   if (!rc) {
      if (hptr) WinDestroyPointer(hptr);
      return NULLHANDLE;
   } /* endif */
   return hptr;
}


LONG loadStr(HWND hwnd, ULONG idstr, PSZ psz) {
   return WinLoadString(WinQueryAnchorBlock(hwnd),
                        NULLHANDLE, idstr, 256, psz);
}


VOID WsetCtlTxt(HWND hwnd, ULONG idctl, ULONG idstr, PSZ pappend) {
   CHAR buf[512];
   if (!loadStr(hwnd, idstr, buf)) buf[0] = 0;
   if (pappend) strcat(buf, pappend);
   WinSetDlgItemText(hwnd, idctl, buf);
}
