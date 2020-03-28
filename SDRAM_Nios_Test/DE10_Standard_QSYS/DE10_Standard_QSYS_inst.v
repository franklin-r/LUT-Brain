	DE10_Standard_QSYS u0 (
		.clk_clk                        (<connected-to-clk_clk>),                        //                     clk.clk
		.clk_sdram_clk                  (<connected-to-clk_sdram_clk>),                  //               clk_sdram.clk
		.key_external_connection_export (<connected-to-key_external_connection_export>), // key_external_connection.export
		.pll_locked_export              (<connected-to-pll_locked_export>),              //              pll_locked.export
		.reset_reset_n                  (<connected-to-reset_reset_n>),                  //                   reset.reset_n
		.sdram_wire_addr                (<connected-to-sdram_wire_addr>),                //              sdram_wire.addr
		.sdram_wire_ba                  (<connected-to-sdram_wire_ba>),                  //                        .ba
		.sdram_wire_cas_n               (<connected-to-sdram_wire_cas_n>),               //                        .cas_n
		.sdram_wire_cke                 (<connected-to-sdram_wire_cke>),                 //                        .cke
		.sdram_wire_cs_n                (<connected-to-sdram_wire_cs_n>),                //                        .cs_n
		.sdram_wire_dq                  (<connected-to-sdram_wire_dq>),                  //                        .dq
		.sdram_wire_dqm                 (<connected-to-sdram_wire_dqm>),                 //                        .dqm
		.sdram_wire_ras_n               (<connected-to-sdram_wire_ras_n>),               //                        .ras_n
		.sdram_wire_we_n                (<connected-to-sdram_wire_we_n>),                //                        .we_n
		.vga_output_BLANK               (<connected-to-vga_output_BLANK>),               //              vga_output.BLANK
		.vga_output_B                   (<connected-to-vga_output_B>),                   //                        .B
		.vga_output_CLK                 (<connected-to-vga_output_CLK>),                 //                        .CLK
		.vga_output_G                   (<connected-to-vga_output_G>),                   //                        .G
		.vga_output_HS                  (<connected-to-vga_output_HS>),                  //                        .HS
		.vga_output_R                   (<connected-to-vga_output_R>),                   //                        .R
		.vga_output_SYNC                (<connected-to-vga_output_SYNC>),                //                        .SYNC
		.vga_output_VS                  (<connected-to-vga_output_VS>)                   //                        .VS
	);

