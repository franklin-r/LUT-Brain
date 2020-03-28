Library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

LIBRARY lpm; 
USE lpm.lpm_components.all; 

ENTITY vga is
   GENERIC (base_address : integer := 0);
   PORT(
	-- Avalon side
	av_reset		: in std_logic;
	av_clk		    : in std_logic;
	av_address  : out std_logic_vector(31 downto 0);
	av_read : out std_logic;
	av_byteenable : out std_logic_vector(16-1 downto 0);
	av_readdata : in std_logic_vector(128-1 downto 0);
	av_readdatavalid    : in std_logic;
	av_waitrequest : in std_logic;

	avs_address  : in std_logic_vector(3 downto 0);
	avs_write : in std_logic;
	avs_writedata : in std_logic_vector(31 downto 0);

	-- VGA side
	CLOCK_25	: in std_logic;
	VGA_BLANK	: out std_logic;
	VGA_B		: out std_logic_vector(9 downto 0);
	VGA_CLK		: out std_logic;
	VGA_G		: out std_logic_vector(9 downto 0);
	VGA_HS		: out std_logic;
	VGA_R		: out std_logic_vector(9 downto 0);
	VGA_SYNC	: out std_logic;
	VGA_VS		: out std_logic
	)
;
END vga;

ARCHITECTURE JPD of vga is
    SIGNAL current_base_address : STD_LOGIC_VECTOR(31 DOWNTO 0);
    SIGNAL next_base_address : STD_LOGIC_VECTOR(31 DOWNTO 0);

	TYPE sm1_type is (idle, read1, read2);
	SIGNAL sm1 : sm1_type;
	SIGNAL current_color : STD_LOGIC_VECTOR(7 downto 0);

	SIGNAL nreset,sync_fifo_out : STD_LOGIC;
	SIGNAL fifo_out_write, fifo_out_read, fifo_out_full, fifo_out_empty : STD_LOGIC;
	SIGNAL fifo_out_input : STD_LOGIC_VECTOR(128-1 DOWNTO 0);
	SIGNAL fifo_out_output : STD_LOGIC_VECTOR(31 DOWNTO 0);
	SIGNAL fifo_out_usedw : STD_LOGIC_VECTOR(7 DOWNTO 0);

	SIGNAL video_on, video_on_v, video_on_h : STD_LOGIC;
	SIGNAL h_count, v_count : STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL H_SYNC, V_SYNC : STD_LOGIC;

	SIGNAL read_address : STD_LOGIC_VECTOR(14 downto 0);
    SIGNAL fifo_cnt : STD_LOGIC_VECTOR(3 downto 0);

	-- COLOR INDEXES
	CONSTANT WHITE_COLORINDEX : 	std_logic_vector(2 downto 0) := "000";
	CONSTANT RED_COLORINDEX : 		std_logic_vector(2 downto 0) := "001";
	CONSTANT GREEN_COLORINDEX: 		std_logic_vector(2 downto 0) := "010";
	CONSTANT BLUE_COLORINDEX : 		std_logic_vector(2 downto 0) := "011";
	CONSTANT YELLOW_COLORINDEX : 	std_logic_vector(2 downto 0) := "100";
	CONSTANT BROWN_COLORINDEX : 	std_logic_vector(2 downto 0) := "101";
	CONSTANT CYAN_COLORINDEX : 		std_logic_vector(2 downto 0) := "110";
	CONSTANT PURPLE_COLORINDEX : 	std_logic_vector(2 downto 0) := "111";
	
BEGIN

process (av_reset, av_clk)
	variable next_read_address : std_logic_vector(18 downto 0);
    begin
	if (av_reset='1') then
		next_base_address <= conv_std_logic_vector(base_address,32);
	elsif (av_clk'event and av_clk='1') then
		if (avs_write = '1') and (avs_address="00") then
			next_base_address <= avs_writedata;
		end if;
	end if;
end process;		

FIFO_OUT:	LPM_FIFO_DC
	generic map (
		LPM_WIDTH => 32,
        LPM_WIDTHU => 8,
		LPM_NUMWORDS => 256
	)

	port map (
		DATA => fifo_out_input(31 downto 0),
		WRCLOCK => av_clk,
		RDCLOCK => clock_25,
		WRREQ => fifo_out_write,
		RDREQ => fifo_out_read,
		ACLR => av_reset,
		Q => fifo_out_output,
		WRUSEDW => fifo_out_usedw,
        WRFULL => fifo_out_full,
        RDEMPTY => fifo_out_empty
	);
	
process (av_reset,av_clk)
	variable next_read_address : std_logic_vector(18 downto 0);
    begin
	if (av_reset='1') then
		current_base_address <= conv_std_logic_vector(base_address,32);
		av_address <=  (others => '0');
		av_read <= '0';
		read_address <= (others => '0');
		fifo_out_write <= '0';
		sm1 <= idle;

	elsif (av_clk'event and av_clk='1') then
		case sm1 is 
		when idle =>
			if (fifo_out_usedw<240) then
				if (read_address = 19199) then 
					current_base_address <= next_base_address;
					read_address <= (others => '0');
				else
					read_address <= read_address + 1;
				end if;
				
				av_address <= current_base_address + (read_address & "0000");
				av_read <= '1';
				sm1 <= read1;
			end if;
			
		when read1 =>
			if (av_waitrequest = '0') then
				av_read <= '0';
			end if;
			
			if (av_readdatavalid ='1') then
				fifo_out_input <= av_readdata;
				av_read <= '0';
				fifo_out_write <='1';
				fifo_cnt <= (others => '0');
				sm1 <= read2;
			end if;

		when read2 =>
			if (fifo_out_full = '0') then
				if (fifo_cnt = "0011") then
					fifo_out_write <='0';
					sm1 <= idle;
				else
					fifo_out_input <= x"00000000" & fifo_out_input(128-1 downto 32);
					fifo_cnt <= fifo_cnt + 1;
				end if;
			end if;
		end case;
	end if;
end process;		

av_byteenable <= (others => '1');

process (av_reset, clock_25)
    begin
	if (av_reset = '1') then nreset <= '0';
	elsif (clock_25'event and clock_25='1') then
		nreset <= '1';
	end if;
end process;

process (nreset,clock_25)
    begin
	if (nreset='0') then
		fifo_out_read <= '0';
		sync_fifo_out <= '0';
	elsif (clock_25'event and clock_25='1') then
		if ((v_count = 479) and (h_count = 638) and (fifo_out_empty = '0')) then
			sync_fifo_out <= '1';
		end if;
		if ( (v_count < 480) and (h_count < 640) and (h_count(1 downto 0) = "10") ) then
	   		fifo_out_read <= sync_fifo_out;
		else 
			fifo_out_read <= '0';
		end if;
	end if;
end process;

current_color <= fifo_out_output(7 downto 0) when (h_count(1 downto 0) = "00") else 
                 fifo_out_output(15 downto 8) when (h_count(1 downto 0) = "01") else 
                 fifo_out_output(23 downto 16) when (h_count(1 downto 0) = "10") else 
                 fifo_out_output(31 downto 24);

process (nreset,clock_25)
    begin
	if (nreset='0') then
	VGA_R <= (others => '0');
	VGA_G <= (others => '0');
	VGA_B <= (others => '0');

	elsif (clock_25'EVENT) AND (clock_25='1') then
		VGA_R <= (others => '0');
		VGA_G <= (others => '0');
		VGA_B <= (others => '0');
		if (video_on = '1') then
			case current_color(7 downto 5) is
				when WHITE_COLORINDEX => 
					VGA_R <= current_color(4 downto 0) & "00000";
					VGA_G <= current_color(4 downto 0) & "00000";
					VGA_B <= current_color(4 downto 0) & "00000";
				when RED_COLORINDEX => 
					VGA_R <= current_color(4 downto 0) & "00000";					
				when GREEN_COLORINDEX => 					
					VGA_G <= current_color(4 downto 0) & "00000";					
				when BLUE_COLORINDEX => 					
					VGA_B <= current_color(4 downto 0) & "00000";
				when YELLOW_COLORINDEX => 
					VGA_R <= current_color(4 downto 0) & "00000";
					VGA_G <= current_color(4 downto 0) & "00000";					
				when BROWN_COLORINDEX => 
					VGA_R <= "0" & current_color(4 downto 0) & "0000";
					VGA_G <= "00" & current_color(4 downto 0) & "000";
					VGA_B <= "0000" & current_color(4 downto 0) & "0";
				when CYAN_COLORINDEX => 					
					VGA_G <= current_color(4 downto 0) & "00000";
					VGA_B <= current_color(4 downto 0) & "00000";
				when PURPLE_COLORINDEX => 
					VGA_R <= current_color(4 downto 0) & "00000";					
					VGA_B <= current_color(4 downto 0) & "00000";	
			end case;
			-- JP Quick patch for grey scales
			VGA_R <= current_color & "00";
			VGA_G <= current_color & "00";
			VGA_B <= current_color & "00";

		end if;
	end if;
end process;

-- video_on is high only when RGB data is displayed
video_on <= video_on_H AND video_on_V;

process (nreset,clock_25)
    begin
	if (nreset='0') then
		H_SYNC <= '1';
		V_SYNC <= '1';
		h_count <= (others => '0');
		v_count <= "0111011111";
		video_on_h <= '1';
		video_on_v <= '1';
	elsif (clock_25'EVENT) AND (clock_25='1') then

--Generate Horizontal and Vertical Timing Signals for Video Signal
-- H_count counts pixels (640 + extra time for sync signals)
-- 
--  Horiz_sync  ------------------------------------__________--------
--  H_count       0                640             659       755    799
--
	IF (h_count = 799) THEN
   		h_count <= "0000000000";
	ELSE
   		h_count <= h_count + 1;
	END IF;

--Generate Horizontal Sync Signal using H_count
	IF (h_count <= 755) AND (h_count >= 659) THEN
 	  	H_SYNC <= '0';
	ELSE
 	  	H_SYNC <= '1';
	END IF;

--V_count counts rows of pixels (480 + extra time for sync signals)
--  
--  Vert_sync      -----------------------------------------------_______------------
--  V_count         0                                      480    493-494          524
--
	IF (v_count = 524) AND (h_count = 699) THEN
   		v_count <= "0000000000";
	ELSE
		IF (h_count = 699) THEN
	   		v_count <= v_count + 1;
		END IF;
	END IF;

-- Generate Vertical Sync Signal using V_count
	IF (v_count <= 494) AND (v_count >= 493) THEN
   		V_SYNC <= '0';
	ELSE
  		V_SYNC <= '1';
	END IF;

-- Generate Video on Screen Signals for Pixel Data
	IF (h_count = 799) THEN
   		video_on_h <= '1';
	ELSIF (h_count = 639) THEN
	   	video_on_h <= '0';
	END IF;

	IF (h_count = 699) THEN
		IF (v_count = 524) THEN
			video_on_v <= '1';
		ELSIF (v_count = 479) THEN
	   		video_on_v <= '0';
		END IF;
	END IF;

    END IF;
END PROCESS;

VGA_CLK <= not clock_25;
VGA_HS <= H_SYNC;
VGA_VS <= V_SYNC;
VGA_BLANK <= H_SYNC and V_SYNC;
VGA_SYNC <= '0';

END JPD;
