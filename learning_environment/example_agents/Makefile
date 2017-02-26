#Call with make <module>.so if module.c is your module's name

#Set to the learning environment directory in the mame source tree.  
# Should be the only variable you are likely to have to set
MAME_LE_DIRECTORY=..

# Enable/disable sending of memory in the Python module
# Disable for speed, enable for debugging, and working out score and game over locations
PYTHON_SEND_MEMORY=0

CFLAGS += -Wall -fPIC -I$(MAME_LE_DIRECTORY)


PYTHONCFLAGS := -I$(shell python -c 'import distutils.sysconfig; print distutils.sysconfig.get_python_inc()')
PYTHONLIBS = -lpython$(shell python -c 'import distutils.sysconfig; print distutils.sysconfig.get_python_version()') -lm

ifeq (1,${PYTHON_SEND_MEMORY})
PYTHONCFLAGS += -DLE_SEND_MEMORY=1
endif

OPTFLAGS = -O2

DEBUGFLAGS = -g

all: pythonbinding.so sit.so


pythonbinding.o: pythonbinding.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) $(PYTHONCFLAGS) $<

pythonbinding.so: pythonbinding.o
	$(CC) -shared $(CFLAGS) $(OPTFLAGS) -o $@ $< $(PYTHONLIBS)

clean:
	rm -f *.o *.so *~

%.o: $(CC) -c $(CFLAGS) $(OPTFLAGS)

%.od: $(CC) -c $(CFLAGS) $(DEBUGFLAGS)

%.so: %.o
	$(CC) -shared $(CFLAGS) $(OPTFLAGS) -o $@ $<

%.sod: %.od
	$(CC) -shared $(CFLAGS) $(DEBUGFLAGS) -o $@ $<
