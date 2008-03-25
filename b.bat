@python assy.py
@del vm.exe
@gcc -o vm.exe -DDBG vm.c -lalleg -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound
vm