	component DE10_Standard_QSYS is
		port (
			clk_clk                        : in    std_logic                     := 'X';             -- clk
			clk_sdram_clk                  : out   std_logic;                                        -- clk
			key_external_connection_export : in    std_logic_vector(3 downto 0)  := (others => 'X'); -- export
			pll_locked_export              : out   std_logic;                                        -- export
			reset_reset_n                  : in    std_logic                     := 'X';             -- reset_n
			sdram_wire_addr                : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_wire_ba                  : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_wire_cas_n               : out   std_logic;                                        -- cas_n
			sdram_wire_cke                 : out   std_logic;                                        -- cke
			sdram_wire_cs_n                : out   std_logic;                                        -- cs_n
			sdram_wire_dq                  : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
			sdram_wire_dqm                 : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_wire_ras_n               : out   std_logic;                                        -- ras_n
			sdram_wire_we_n                : out   std_logic;                                        -- we_n
			vga_output_BLANK               : out   std_logic;                                        -- BLANK
			vga_output_B                   : out   std_logic_vector(9 downto 0);                     -- B
			vga_output_CLK                 : out   std_logic;                                        -- CLK
			vga_output_G                   : out   std_logic_vector(9 downto 0);                     -- G
			vga_output_HS                  : out   std_logic;                                        -- HS
			vga_output_R                   : out   std_logic_vector(9 downto 0);                     -- R
			vga_output_SYNC                : out   std_logic;                                        -- SYNC
			vga_output_VS                  : out   std_logic                                         -- VS
		);
	end component DE10_Standard_QSYS;

	u0 : component DE10_Standard_QSYS
		port map (
			clk_clk                        => CONNECTED_TO_clk_clk,                        --                     clk.clk
			clk_sdram_clk                  => CONNECTED_TO_clk_sdram_clk,                  --               clk_sdram.clk
			key_external_connection_export => CONNECTED_TO_key_external_connection_export, -- key_external_connection.export
			pll_locked_export              => CONNECTED_TO_pll_locked_export,              --              pll_locked.export
			reset_reset_n                  => CONNECTED_TO_reset_reset_n,                  --                   reset.reset_n
			sdram_wire_addr                => CONNECTED_TO_sdram_wire_addr,                --              sdram_wire.addr
			sdram_wire_ba                  => CONNECTED_TO_sdram_wire_ba,                  --                        .ba
			sdram_wire_cas_n               => CONNECTED_TO_sdram_wire_cas_n,               --                        .cas_n
			sdram_wire_cke                 => CONNECTED_TO_sdram_wire_cke,                 --                        .cke
			sdram_wire_cs_n                => CONNECTED_TO_sdram_wire_cs_n,                --                        .cs_n
			sdram_wire_dq                  => CONNECTED_TO_sdram_wire_dq,                  --                        .dq
			sdram_wire_dqm                 => CONNECTED_TO_sdram_wire_dqm,                 --                        .dqm
			sdram_wire_ras_n               => CONNECTED_TO_sdram_wire_ras_n,               --                        .ras_n
			sdram_wire_we_n                => CONNECTED_TO_sdram_wire_we_n,                --                        .we_n
			vga_output_BLANK               => CONNECTED_TO_vga_output_BLANK,               --              vga_output.BLANK
			vga_output_B                   => CONNECTED_TO_vga_output_B,                   --                        .B
			vga_output_CLK                 => CONNECTED_TO_vga_output_CLK,                 --                        .CLK
			vga_output_G                   => CONNECTED_TO_vga_output_G,                   --                        .G
			vga_output_HS                  => CONNECTED_TO_vga_output_HS,                  --                        .HS
			vga_output_R                   => CONNECTED_TO_vga_output_R,                   --                        .R
			vga_output_SYNC                => CONNECTED_TO_vga_output_SYNC,                --                        .SYNC
			vga_output_VS                  => CONNECTED_TO_vga_output_VS                   --                        .VS
		);

