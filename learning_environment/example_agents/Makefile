#Call with make <module>.so if module.c is your module's name

#Set to the learning environment directory in the mame source tree.  
# Should be the only variable you are likely to have to set
MAME_LE_DIRECTORY=..

CFLAGS += -Wall -fPIC -I$(MAME_LE_DIRECTORY)


PYTHON2CFLAGS := -I$(shell python2 -c 'import sysconfig; print(sysconfig.get_config_var("INCLUDEPY"))')
PYTHON2LIBS := -l$(shell python2 -c 'import sysconfig; print (sysconfig.get_config_var("LDLIBRARY")[3:-3])') -lm

PYTHON3CFLAGS := -I$(shell python3 -c 'import sysconfig; print(sysconfig.get_config_var("INCLUDEPY"))')
PYTHON3LIBS := -l$(shell python3 -c 'import sysconfig; print (sysconfig.get_config_var("LDLIBRARY")[3:-3])') -lm

OPTFLAGS = -O2

DEBUGFLAGS = -g


# Conditionally compile python2 and python3 bindings depending on what's installed
PYTHONBINDINGS = 
ifneq ($(PYTHON2LIBS), "-l -lm")
	PYTHONBINDINGS += python2binding.so
endif

ifneq ($(PYTHON3LIBS), "-l -lm")
	PYTHONBINDINGS += python3binding.so
endif

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
