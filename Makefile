ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
MKSPRITE = $(ROOTDIR)/bin/mksprite
N64TOOL = $(ROOTDIR)/bin/n64tool
LINK_FLAGS = -L$(ROOTDIR)/lib -L$(ROOTDIR)/mips64-elf/lib -ldragon -lmikmod -lc -lm -ldragonsys -Tn64.ld
PROG_NAME = minesweeper-64
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/mips64-elf/include -Iinclude -I/usr/local/include/
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

all: build
	
build: ##    Create rom.
	@docker --version &> /dev/null
	@if [ $$? -ne 0 ]; then echo "Building rom..." && make $(PROG_NAME).z64; fi
	@if [ $$? -eq 0 ]; then echo "Building rom inside docker environment..." && make docker; fi

docker: setup
	@docker run -v ${CURDIR}:/game build make $(PROG_NAME).z64

rebuild: clean build	##  Erase temp files and create rom.

# gfx #
PNGS := $(wildcard resources/gfx/*.png)
SPRITES := $(subst .png,.sprite,$(subst resources/,filesystem/,$(PNGS)))
filesystem/gfx/%.sprite: resources/gfx/%.png
	@mkdir -p `echo $@ | xargs dirname`
	$(MKSPRITE) 16 1 1 $< $@

# sfx #
MODS := $(wildcard resources/sfx/*.mod)
BGMS := $(subst resources/,filesystem/,$(MODS))
filesystem/sfx/%.mod: resources/sfx/%.mod
	@mkdir -p `echo $@ | xargs dirname`
	cp $< $@

# code #
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $@ $^ $(LINK_FLAGS)

$(PROG_NAME).bin : $(PROG_NAME).elf
	$(OBJCOPY) -O binary $< $@

# dfs #
$(PROG_NAME).dfs: $(SPRITES) $(BGMS)
	@mkdir -p ./filesystem/
	@echo `git rev-parse HEAD` > ./filesystem/hash
	$(MKDFSPATH) $@ ./filesystem/

$(PROG_NAME).z64: $(PROG_NAME).bin $(PROG_NAME).dfs
	@rm -f $@
	$(N64TOOL) -l 4M -t "$(PROG_NAME)" -h $(ROOTDIR)/mips64-elf/lib/header -o $(PROG_NAME).z64 $(PROG_NAME).bin -s 1M $(PROG_NAME).dfs
	$(CHKSUM64PATH) $@

setup:		##    Create dev environment (docker image).
	@docker build -q -t build  - < Dockerfile > /dev/null

resetup:	##  Force recreate the dev environment (docker image).
	@echo "Rebuilding dev environment in docker..."
	@ docker build -q -t build  --no-cache  - < Dockerfile > /dev/null

cen64:		##    Start rom in CEN64 emulator.
	@echo "Starting cen64..."
	$(CEN64_DIR)/cen64 -multithread -controller num=1,pak=rumble $(CEN64_DIR)/pifdata.bin $(PROG_NAME).z64

clean:		##    Cleanup temp files.
	@echo "Cleaning up temp files..."
	rm -rf *.z64 *.elf src/*.o *.bin *.dfs ./filesystem

help:		##     Show this help.
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/:.*##/:/' 
