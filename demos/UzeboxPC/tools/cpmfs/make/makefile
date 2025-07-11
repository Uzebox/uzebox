
PROG    = cpmfs
CC      = gcc
ZIP     = "C:\Program Files\7-Zip\7z.exe"

INC    := ../include
SRC    := ../sources
CFLAGS  =
LFLAGS  =



all : cpmfs.exe

cpmfs.exe : cpmfs.o misc.o os_win32.o
	$(CC) $^ $(LFLAGS) -o $@

test : cpmfs.exe
	if [ -d /c/test/${PROG} ]; then rm -r /c/test/${PROG}; fi
	mkdir /c/test/${PROG}
	cp cpmfs.exe /c/test/${PROG}
	cp -r ../misc/test/* /c/test/${PROG}
	(cd /c/test/${PROG}; echo exit after testing; sh)

install : cpmfs.exe
	cp $< ${USERPROFILE}/bin

clean :
	-rm ${PROG}.exe *.o

snapshot :
	$(eval now  = `date +"%Y-%m-%d.%H%M"`)
	(cd ../..; ${ZIP} a ${PROG}-${now}.zip ${PROG}\\ )




# Binaries
# ########

# $@ is the target
# $^ is a list of all the prerequisites of the rule, including the names of the directories in which they were found
# $< is just the first prerequisite
# $+ is all prerequisites in order listed, even dupes, even if up to date


%.o : ${SRC}/%.c ${INC}/misc.h
	gcc -o $@ ${CFLAGS} -I${INC} -c $< 

