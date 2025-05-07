# when you want to change the python version, you must update the d.lst
# in the python project accordingly !!!
PYMAJOR=2
PYMINOR=6
PYMICRO=1
PYVERSION=$(PYMAJOR).$(PYMINOR).$(PYMICRO)

.IF "$(GUI)" == "UNX"
.IF "$(OS)" == "MACOSX"
PY_FULL_DLL_NAME=libpython$(PYMAJOR).$(PYMINOR).a
PYTHONLIB=-F$(SOLARLIBDIR) -framework OOoPython
CFLAGS+=-I$(SOLARLIBDIR)/OOoPython.framework/Versions/$(PYMAJOR).$(PYMINOR)/include/python$(PYMAJOR).$(PYMINOR)
.ELSE
PY_FULL_DLL_NAME=libpython$(PYMAJOR).$(PYMINOR).so.1.0
PYTHONLIB=-lpython$(PYMAJOR).$(PYMINOR)
.ENDIF
.ELSE
.IF "$(COM)" == "GCC"
PY_FULL_DLL_NAME=libpython$(PYMAJOR).$(PYMINOR).dll
PYTHONLIB=-lpython$(PYMAJOR).$(PYMINOR)
.ELSE
PY_FULL_DLL_NAME=python$(PYMAJOR)$(PYMINOR).dll
PYTHONLIB=python$(PYMAJOR)$(PYMINOR).lib
.ENDIF
.ENDIF
