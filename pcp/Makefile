src=./src/
lib=./lib/
include=./include/
conf=./configs/


all: launcher ear brain mouth ui
#	./launcher ${conf}config

launcher: ${src}launcher.c
	gcc -o launcher ${src}launcher.c -lm

brainutil.o:${src}brainutil.c
	gcc -c ${src}brainutil.c -I ${include}

ear: ${src}ear.c
	gcc -o ear ${src}ear.c
mouth: ${src}mouth.c
	gcc -o mouth ${src}mouth.c
brain: ${src}brain.c brainutil.o
	gcc -o brain ${src}brain.c brainutil.o ${lib}ecdc.a -lm -lpthread -I ${include}
ui: ${src}ui.c
	gcc -o ui ${src}ui.c -I ${include}

clean:
	rm -f ear mouth ui brain launcher brainutil.o *logs
	ls configs | sed 's;\(.*\);sed -n '"'"'4,7p '"'"' configs/\1;'  | sh | sed 's;\(.*\);rm -f \1;' | sh
	#sh killer
	echo "Deleted"
	#make
