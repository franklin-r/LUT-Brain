# Create the work library named work
vlib work

# Compile the VHDL files
vcom -93 ../src/lutForward.vhd
vcom -93 ../simu/lutForward_tb.vhd

# Choose the entity to simulate
vsim lutForward_tb(bench)

# Show the signals
view signals
add wave lutForward_tb/UUT/clk
add wave lutForward_tb/UUT/clk_en
add wave lutForward_tb/UUT/reset
add wave lutForward_tb/UUT/start

add wave lutForward_tb/UUT/c_state
add wave -radix unsigned lutForward_tb/UUT/i
add wave -radix hexadecimal lutForward_tb/UUT/l_LUT_array
add wave -radix hexadecimal lutForward_tb/UUT/l_value
add wave -radix hexadecimal lutForward_tb/UUT/l_LUT_Address

add wave -radix hexadecimal lutForward_tb/UUT/dataa
add wave -radix hexadecimal lutForward_tb/UUT/datab
add wave lutForward_tb/UUT/n

add wave lutForward_tb/UUT/waitrequest
add wave lutForward_tb/UUT/done
add wave -radix unsigned lutForward_tb/UUT/result

add wave -radix hexadecimal lutForward_tb/UUT/addr
add wave -radix hexadecimal lutForward_tb/UUT/readdata
add wave -radix hexadecimal lutForward_tb/UUT/writedata
add wave lutForward_tb/UUT/wr
add wave lutForward_tb/UUT/rd

# Run the simulation
run -all



