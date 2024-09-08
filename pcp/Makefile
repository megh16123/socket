src=./src/
lib=./lib/
include=./include/
conf=./configs/


all: launcher ear brain mouth ui
#	./launcher ${conf}config

launcher: ${src}launcher.c
	gcc -o launcher ${src}launcher.c

ear: ${src}ear.c
	gcc -o ear ${src}ear.c
mouth: ${src}mouth.c
	gcc -o mouth ${src}mouth.c
brain: ${src}brain.c
	gcc -o brain ${src}brain.c ${lib}ecdc.a -lm -I ${include}
ui: ${src}ui.c
	gcc -o ui ${src}ui.c

clean:
	rm -f ear mouth ui brain launcher
	ls configs | sed 's;\(.*\);sed -n '"'"'4,7p '"'"' configs/\1;'  | sh | sed 's;\(.*\);rm -f \1;' | sh
	sh killer
	echo "Deleted"
