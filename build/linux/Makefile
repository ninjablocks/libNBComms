CC     := cc

INCLUDEDIR	:= ../../include/

CFLAGS	:= -Wall -fPIC -c
INCLUDES	:= -I$(INCLUDEDIR)
SOURCES := $(shell echo ../../src/*.c)
OBJECTS := $(SOURCES:.c=.o)

HEADERS	:= $(INCLUDEDIR)nbsocket.h

TARGET	:= libnbcomms.so
TARGETVER	:= $(TARGET).1.0

LDFLAGS	:= -shared

LIBINCLUDE	:= ../../../include/
LIBDIR	:= ../../../lib/linux/

all: $(TARGETVER)

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(TARGETVER): $(OBJECTS)
	$(CC) $(LDFLAGS) -Wl,-soname,$(TARGET).1 -o $(TARGETVER) $(OBJECTS)

install:
	cp $(HEADERS) $(LIBINCLUDE)
	mv $(TARGETVER) $(LIBDIR)
	ln -sf $(LIBDIR)$(TARGETVER) $(LIBDIR)$(TARGET).1
	ln -sf $(LIBDIR)$(TARGET).1 $(LIBDIR)$(TARGET)
	@echo " - Done."
	@echo "Please ensure that the environment variable, LD_LIBRARY_PATH, contains $(LIBDIR)\n"

clean:
	rm -rf $(OBJECTS) $(TARGETVER) $(LIBDIR)$(TARGETVER)

	