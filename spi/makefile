SRC_FILES = *.v

all: $(SRC_FILES)
	iverilog -o wave $(SRC_FILES)
	vvp -n wave -lxt2
	gtkwave wave.vcd