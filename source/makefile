
CC = icc.exe
CL = ilink.exe
RC = rc.exe
RB = rc.exe

# compiler and linker flags

# Debug version
!ifdef DEBUG

CFLAGS = /I..\headers /I..\APIexH /Ss /Ti /Rn /G5 /C /Q
LFLAGS = /DE /E:2 /PM:PM /PACKD /A:4 /OPTF /NOL /M /L

!else
# RELEASE version

CFLAGS = /I..\headers /I..\APIexH /Ss /O /Oc /Ol /Rn /G5 /C /Q
LFLAGS = /E:2 /PM:PM /PACKD /A:4 /OPTF /NOL /M /L

!endif

.C.obj:
   $(CC) $(CFLAGS) $<

all: thememgr.exe

thememgr.exe:        \
   thememgr.obj      \
   util.obj          \
   dllapi.obj
   $(CL) @<<
      $(LFLAGS)
      /O:thememgr.exe
      thememgr.obj
      util.obj
      dllapi.obj
    <<
thememgr.obj      : thememgr.c        *.h
util.obj          : util.c            *.h
dllapi.obj        : dllapi.c          *.h

