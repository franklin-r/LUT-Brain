transcript on
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vcom -93 -work work {C:/Users/Louis-Normand/Documents/Travail/LUT-Brain/SDRAM_Nios_Test/hdl/buildAddress.vhd}

vcom -93 -work work {C:/Users/Louis-Normand/Documents/Travail/LUT-Brain/SDRAM_Nios_Test/hdl/buildAddress.vhd}
vcom -93 -work work {C:/Users/Louis-Normand/Documents/Travail/LUT-Brain/SDRAM_Nios_Test/hdl/buildAddress_tb.vhd}

vsim -t 1ps -L altera -L lpm -L sgate -L altera_mf -L altera_lnsim -L cyclonev -L rtl_work -L work -voptargs="+acc"  buildAddress_tb

add wave *
view structure
view signals
run 10 us