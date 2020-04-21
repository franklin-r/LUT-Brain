library ieee;
use ieee.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;


entity buildAddress is
port(
	--Clock and reset signals
	signal clk    : in std_logic;  -- CPU system clock (always required)
	signal clk_en : in std_logic;	 -- Clock-qualifier (always required)
	signal reset  : in std_logic;	 -- CPU master asynchronous active high reset (always required)
	
	--Avalon memory interface
	signal addr     : out std_logic_vector(31 downto 0);
	signal rd       : out std_logic;
	signal wr 	    : out std_logic;
	signal wrdata   : out std_logic_vector(31 downto 0);
	signal rddata   : in std_logic_vector(31 downto 0);
	signal busy     : in std_logic;
	
	--Custom instruction signals
	signal start  : in std_logic;	 -- Active high signal used to specify that inputs are valid
	signal done   : out std_logic; -- Active high signal used to notify the CPU that result is valid
	signal dataa  : in std_logic_vector(31 downto 0);  -- Operand A (always required)
	signal datab  : in std_logic_vector(31 downto 0);  -- Operand B (optional)
	signal result : out std_logic_vector(31 downto 0);	-- result (always required)
   signal n      : in std_logic_vector(1 downto 0)	   -- Operation extension - 4 modes support
);
end entity buildAddress;

architecture a_custominstruction of buildAddress is

	-- Internal signals
	--
	signal n_neuron           : std_logic_vector(31 downto 0);
	signal n_input_per_neuron : std_logic_vector(31 downto 0);
	signal source             : std_logic_vector(31 downto 0);
	signal LUT_Address        : std_logic_vector(31 downto 0);
	signal current_pos        : std_logic_vector(31 downto 0);
	
	signal source_data        : std_logic_vector(31 downto 0);
	signal LUT_Address_data   : std_logic_vector(31 downto 0);
	signal current_pos_data   : std_logic_vector(31 downto 0);
	
	signal i,j                : std_logic_vector(31 downto 0);
	signal current_pos_index  : std_logic_vector(31 downto 0);
	
	-- States:
	type t_etat is (INIT,S1,S2,S4,RDRQ1,RDRQ2,RDRQ3,WRRQ); 
	signal etat : t_etat;
begin
	-- Combinatory logic
	--
	-- Custom Instruction state machine
	process( clk, reset )
	begin
		if( reset = '1' ) then
		    done <= '0';
			 result  <= (others => '0');   
			 wr <= '0';
			 rd <= '0';
			 i  <= (others => '0');
			 j  <= (others => '0');
   		 current_pos_index <= (others => '0');
			 
			 etat <= INIT;
		elsif(rising_edge(clk)) then
			case etat is 
			
			   --Initialization
				when INIT  =>
				   done <= '0';
				   result  <= (others => '0');   

					--Wait for stable inputs
					if start = '0' then etat <= INIT;
					else 
						case n is
							when "00" =>
								n_neuron 		    <= dataa;
								n_input_per_neuron <= datab;
								done 					 <= '1';
								etat 					 <= INIT;
							when "01" =>
								source 	   <= dataa;
								LUT_Address <= datab;
								done        <= '1';
								etat        <= INIT;			
							when "10" =>
							   current_pos <= dataa;
								current_pos_index <= (others => '0');
								wr          <= '0';
								rd          <= '0'; 
								i           <= (others => '0');
								etat        <= S1;
							when others => 
							   result(0)  <= '1';
							   done       <= '1';
								etat 		  <= INIT;
						end case;
					end if;
					
			   -- Check if layer is fully connected
            when S1    =>
					if i < n_neuron then 
						j    <= (others => '0');
					   LUT_Address_data	<= (others => '0');
						etat <= S2;
					else 
						done   <= '1';
						etat   <= INIT;
				   end if;
					
			   --Check if all neuron inputs are connected
				when S2    =>
					if j < n_input_per_neuron then
				      addr <= current_pos + current_pos_index;
					   rd   <= '1'; 
						etat <= RDRQ1;
					else 
						addr   <= LUT_Address + i;
						wrdata <= LUT_Address_data; 
						wr   <= '1'; 
						etat <= WRRQ;
					end if;
					
				--Check if input needs to be connected	
				when S4    =>
					if source_data = 0 then
					   LUT_Address_data <= LUT_Address_data(30 downto 0) & '0';
					else
						LUT_Address_data <= LUT_Address_data(30 downto 0) & '1';
					end if;
					j                 <= j + 1;
					current_pos_index <= current_pos_index + 1;
					etat              <= S2;
				
				--Wait for current_pos_data	
				when RDRQ1 =>
					if busy = '1' then etat <= RDRQ1;
					else 
						current_pos_data <= rddata;
						addr <= source + current_pos_data; 
						etat <= RDRQ2;
					end if;
				
				--Wait for source_data	
				when RDRQ2 =>
					if busy = '1' then etat <= RDRQ2;
					else 
						source_data <= rddata; 
						rd   <= '0'; 
						etat <= S4;
					end if;
					
				--Wait for LUT_Address_data write
				when WRRQ  =>
					if busy = '1' then etat <= WRRQ;
					else
					   wr   <= '0';
						i    <= i + 1; 
						etat <= S1;
					end if;
					
				when others => 
					done   <= '1';
					etat   <= INIT;
			end case;
		end if;	
	end process;
end architecture a_custominstruction;