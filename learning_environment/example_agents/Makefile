#Call with make <module>.so if module.c is your module's name

#Set to the learning environment directory in the mame source tree.  
# Should be the only variable you are likely to have to set
MAME_LE_DIRECTORY=..

CFLAGS += -Wall -fPIC -I$(MAME_LE_DIRECTORY)



PYTHONBINDINGS = 

ifneq (, $(shell which python2-config))
	PYTHONBINDINGS += python2binding.so
	PYTHON2CFLAGS := $(shell python2-config --includes)
	PYTHON2LIBS := $(shell python2-config --libs)
endif

ifneq (, $(shell which python3-config))
	PYTHONBINDINGS += python3binding.so
	PYTHON3CFLAGS := $(shell python3-config --includes)
	PYTHON3LIBS := $(shell python3-config --libs)
endif

OPTFLAGS = -O2

DEBUGFLAGS = -g


all: sit.so $(PYTHONBINDINGS)

print-%  : ; @echo $* = $($*)

python2binding.o: pythonbinding.c
	$(CC) -o python2binding.o -c $(CFLAGS) $(OPTFLAGS) $(PYTHON2CFLAGS) $<

python3binding.o: pythonbinding.c
	$(CC) -o python3binding.o -c $(CFLAGS) $(OPTFLAGS) $(PYTHON3CFLAGS) $<


python2binding.so: python2binding.o
	$(CC) -shared $(CFLAGS) $(OPTFLAGS) -o $@ $< $(PYTHON2LIBS)

python3binding.so: python3binding.o
	$(CC) -shared $(CFLAGS) $(OPTFLAGS) -o $@ $< $(PYTHON3LIBS)	


clean:
	rm -f *.o *.so *~

%.o: $(CC) -c $(CFLAGS) $(OPTFLAGS)

%.od: $(CC) -c $(CFLAGS) $(DEBUGFLAGS)

%.so: %.o
	$(CC) -shared $(CFLAGS) $(OPTFLAGS) -o $@ $<

%.sod: %.od
	$(CC) -shared $(CFLAGS) $(DEBUGFLAGS) -o $@ $<
