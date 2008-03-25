import array,string
f=file("charset.txt","rt")

chars=[]
cmapping={}

char=[]
cmap=None

for l in f:
	l=l.rstrip()
	if l=="":
		if len(char):
			while len(char)<6:
				char.append("      ")
			#print "Character %d (map=%s):"%(len(chars),cmap)
			#for l in char:
			#	print l
			chars.append(char)
			if cmap: cmapping[cmap]=len(chars)-1
			char=[]
			cmap=None
		
	else:
		if l.startswith("map="):
			cmap=l[4]
		else:
			char.append(l.replace("."," ").rstrip().ljust(6))

def a2c(a):
	c=0
	for i,v in enumerate(a): c|=v<<i
	return c

def bytes(a):
	s=""
	for i in xrange(0,len(a),8):
		p=a[i:i+8]
		while len(p)<8: p.append(0)
		s+=chr(a2c(p))
	return s

cec={0:r"\0",7:r"\a",8:r"\b",9:r"\t",10:r"\n",11:r"\v",12:r"\f",13:r"\r",27:r"\e"}

def crepr(s):
	s=str(s)
	o="\""
	for c in s:
		oc=ord(c)
		if oc in cec:
			o+=cec[oc]
		elif c in string.printable:
			o+=c
		else:
			o+=r"\x%02x"%oc
		
	o+="\""
	return o
			
rom=""
for ch in chars:
	for l in ch:
		v=[]
		for c in l:
			if c!=" ": v.append(1)
			else: v.append(0)
		rom+=bytes(v)
print "%d characters defined, %d bytes"%(len(chars),len(rom))
cgf=open("chargen.h","wt")
print >>cgf, "#ifndef CHARGEN_H\n#define CHARGEN_H"
print >>cgf, "// Chargen ROM. %d characters defined, %d bytes"%(len(chars),len(rom))
print >>cgf, "char *cgrom="+crepr(rom)+";"
print >>cgf, "#endif"
print >>cgf
cgf.close()
csp=open("charset.py","wt")
print >>csp,"# Character mapping."
print >>csp,"charmap="+repr(cmapping)
print >>csp
csp.close()