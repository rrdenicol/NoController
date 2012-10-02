all: nocontroller

nocontroller: utils.h utils.c ofp.h ofp.c nocontroller.h nocontroller.c apps/$(APP).c
	gcc $+ -o nocontroller -lpthread -I.

clean:
	rm -rf nocontroller
