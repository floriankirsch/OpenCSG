SUBDIRS = glew src example

all:
	for X in $(SUBDIRS); do make -C $$X ; done

clean:
	for X in $(SUBDIRS); do make -C $$X clean; done

distclean:
	for X in $(SUBDIRS); do make -C $$X distclean; done
