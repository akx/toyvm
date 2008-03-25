o=set()
for l in file("colors.txt"):
	rgb=map(int,l.strip().split()[:3])
	l=tuple((c/32)*8 for c in rgb)
	o.add(l)
o=list(o)
o.sort(key=lambda x:x[1]+x[2])
print "#ifndef PAL_H"
print "#define PAL_H"
print "unsigned char pal[]={"
for c in o:
	print "\t"+", ".join(map(str,c))+","
print "};"
print "#define PALCNT %d"%len(o)
print "#endif"
print