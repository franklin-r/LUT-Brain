-- File			:	lutForward.vhd
-- Block 		:	lutForward
-- Author 		:	Alexis ROSSI <alexis.rossi@polymtl.ca>
-- Description 	:	Computes the LUTs' output
-- Context 		:	The block's purpose is to accelrate the propagation 
--					the neural network
-- Released		:	17/04/2020
-- Updated		: 		


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity lutForward is port(

	clk 		: in std_logic;							-- CPU system clock (always required)
	clk_en		: in std_logic;							-- Clock-qualifier (always required)
	reset		: in std_logic;							-- CPU master asynchronous active high reset (always required)
	start		: in std_logic;							-- Active high signal used to specify that inputs are valid (always required)
	dataa		: in std_logic_vector(31 downto 0);		-- Operand A (always required)
	datab		: in std_logic_vector(31 downto 0);		-- Operand B (optional)
	n			: in std_logic;

	waitrequest	: in std_logic;
	
	done		: out std_logic;						-- Active high signal used to notify the CPU that result is valid (required for variable multi-cycle)
	result		: out std_logic_vector(31 downto 0);	-- result (always required)

	addr		: out std_logic_vector(31 downto 0);	-- SDRAM's address of the data to load/store
	readdata	: in std_logic_vector(31 downto 0);		-- Data to load
	writedata	: out std_logic_vector(31 downto 0);	-- Data to store
	wr			: out std_logic;						-- Indicate whether we store data or not
	rd			: out std_logic							-- Indicate whether we load data or not
);											
end entity; 


architecture a_custominstruction of lutForward is 

	-- Local custom instruction signals
	type state_t is (INIT, S0, S1, S2, S3, S4);
	
	signal c_state 				: state_t;							-- Current state
	
	signal real_clk				: std_logic;						-- Actual clock of the system
	
	signal LUT_size 			: unsigned(15 downto 0);			-- Size of the LUTs
	signal n_neuron				: unsigned(15 downto 0);			-- Number of neurons in a layer
	
	signal i 					: integer;							-- Counter
	signal tmp					: integer;							-- Temporary value for an operation
	
	signal T0					: boolean;							-- Test condition for 
	signal T1					: boolean;							-- Test condition for 
	
	signal val_LUT_Address		: std_logic_vector(31 downto 0);	-- Saved address of an LUT
	
	signal l_LUT_array			: unsigned(31 downto 0);			-- Base address of LUT_array
	signal l_value				: unsigned(31 downto 0);			-- Base address of value
	signal l_LUT_Address		: unsigned(31 downto 0);			-- Base address of LUT_Address
	
	begin
		real_clk	<= clk and clk_en;
	
		-- Custom instruction logic
		process(real_clk, reset)
		
		begin
			if(reset = '1') then
				done 		<= '0';
				result		<= (others => '0');
				addr		<= (others => '0');
				writedata	<= (others => '0');
				wr			<= '0';
				rd			<= '0';				
				c_state 	<= INIT;
				
			elsif(rising_edge(real_clk)) then
				case c_state is 
					when INIT =>  				-- Initialisation of control signals
						if(start = '0') then
							c_state	<= INIT;
							
						else
							done 		<= '0';
							result		<= (others => '0');
							rd			<= '0';
							wr			<= '0';
							addr		<= (others => '0');
							writedata	<= (others => '0');
							i			<= 0;
							T0			<= (n = '0');
							c_state		<= S0;
						end if;													
					
					when S0 =>					-- Initialisation of data signals
						if(T0) then
							l_LUT_array	<= unsigned(dataa);
							l_value		<= unsigned(datab);
							T1			<= false;
							LUT_size	<= (others => '0');
							n_neuron	<= (others => '0');
							c_state		<= S1;
							
						else
							l_LUT_Address 	<= unsigned(dataa);
							n_neuron		<= unsigned(datab(31 downto 16));
							LUT_size		<= unsigned(datab(15 downto 0));
							T1				<= true;
							c_state			<= S1;
						end if;
						
					when S1	=>					-- Test whether to end the process or not
						rd			<= '0';
						wr			<= '0';
						
						if(T1) then
							tmp		<= to_integer(LUT_size) * i;
							c_state	<= S2;
							
						else
							done 	<= '1';
							c_state	<= INIT;
						end if;
						
					when S2	=>					-- Read LUT_Address[i]
						rd		<= '1';
						wr		<= '0';
						addr	<= std_logic_vector(l_LUT_Address + to_unsigned(i, l_LUT_Address'length));
						
						if(waitrequest = '0') then
							c_state	<= S3;
							
						else
							c_state	<= S2;
						end if;
					
					-- Here, readdata = l_LUT_Address[i] = val_LUT_Address
					when S3	=>					-- Read LUT_array[LUT_size * i + (LUT_Address[i] >> 3)]
						rd				<= '1';
						wr				<= '0';
						addr			<= std_logic_vector(l_LUT_array + to_unsigned(tmp, l_LUT_array'length) + shift_right(unsigned(readdata), 3));
						val_LUT_Address	<= readdata;
						
						if(waitrequest = '0') then
							c_state	<= S4;
							
						else
							c_state	<= S3;
						end if;
					
					-- Here, readdata = l_LUT_array[tmp + (val_LUT_Address >> 3)]
					when others =>	-- S4		-- Write to value[i]
						rd			<= '0';
						wr			<= '1';
						addr		<= std_logic_vector(l_value + to_unsigned(i, l_value'length));
						writedata	<= x"00000001" and std_logic_vector(shift_right(unsigned(readdata), to_integer(unsigned(val_LUT_Address and x"00000007"))));
						
						if(waitrequest = '0') then
							T1		<= ((i + 1) < n_neuron);
							i		<= i + 1;
							c_state	<= S1;
							
						else
							c_state	<= S4;
						end if;
									
				end case;
			end if;	
		end process;
end architecture;