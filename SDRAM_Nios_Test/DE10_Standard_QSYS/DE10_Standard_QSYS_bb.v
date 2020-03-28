
module DE10_Standard_QSYS (
	clk_clk,
	clk_sdram_clk,
	key_external_connection_export,
	pll_locked_export,
	reset_reset_n,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	vga_output_BLANK,
	vga_output_B,
	vga_output_CLK,
	vga_output_G,
	vga_output_HS,
	vga_output_R,
	vga_output_SYNC,
	vga_output_VS);	

	input		clk_clk;
	output		clk_sdram_clk;
	input	[3:0]	key_external_connection_export;
	output		pll_locked_export;
	input		reset_reset_n;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[15:0]	sdram_wire_dq;
	output	[1:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	output		vga_output_BLANK;
	output	[9:0]	vga_output_B;
	output		vga_output_CLK;
	output	[9:0]	vga_output_G;
	output		vga_output_HS;
	output	[9:0]	vga_output_R;
	output		vga_output_SYNC;
	output		vga_output_VS;
endmodule
