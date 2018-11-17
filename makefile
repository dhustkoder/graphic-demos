SUBDIRS= common 01_triangle 02_rotate 03_piramid 04_cube 05_texture 06_cube_texture

all: $(SUBDIRS)
	$(foreach d, $^, $(MAKE) -C $(d);)

clean: $(SUBDIRS)
	$(foreach d, $^, $(MAKE) -C $(d) clean;)


.PHONY: $(SUBDIRS) clean

