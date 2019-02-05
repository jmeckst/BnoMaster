#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

$(call compile_only_if,$(CONFIG_ENABLE_MESH_WIFI),wifi.o)
