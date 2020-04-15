-- File			:	lutForward_tb.vhd
-- Author 		:	Alexis ROSSI <alexis.rossi@polymtl.ca>
-- Description 	:	Testbench of the lutForward block
-- Released		:	
-- Updated		: 		


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity lutForward_tb is
end entity;

architecture bench of lutForward_tb is

  component lutForward
  port(
  	signal clk 			: in std_logic;							-- CPU system clock (always required)
	signal clk_en		: in std_logic;							-- Clock-qualifier (always required)
	signal reset		: in std_logic;							-- CPU master asynchronous active high reset (always required)
	signal start		: in std_logic;							-- Active high signal used to specify that inputs are valid (always required)
	signal dataa		: in std_logic_vector(31 downto 0);		-- Operand A (always required)
	signal datab		: in std_logic_vector(31 downto 0);		-- Operand B (optional)
	signal n			: in std_logic;

	signal waitrequest	: in std_logic;
	
	signal done			: out std_logic;						-- Active high signal used to notify the CPU that result is valid (required for variable multi-cycle)
	signal result		: out std_logic_vector(31 downto 0);	-- result (always required)

	signal addr			: out std_logic_vector(31 downto 0);	-- SDRAM's address of the data to load/store
	signal readdata		: in std_logic_vector(31 downto 0);		-- Data to load
	signal writedata	: out std_logic_vector(31 downto 0);	-- Data to store
	signal wr			: out std_logic;						-- Indicate whether we store data or not
	signal rd			: out std_logic							-- Indicate whether we load data or not
  );
  end component;

	signal clk 			: std_logic := '0';						
	signal clk_en		: std_logic := '1';								
	signal reset		: std_logic := '1';							
	signal start		: std_logic := '0';							
	signal dataa		: std_logic_vector(31 downto 0) := (others => '0');			
	signal datab		: std_logic_vector(31 downto 0) := (others => '0');			
	signal n			: std_logic := '0';

	signal waitrequest	: std_logic := '0';
	
	signal done			: std_logic;							
	signal result		: std_logic_vector(31 downto 0);		

	signal addr			: std_logic_vector(31 downto 0);		
	signal readdata		: std_logic_vector(31 downto 0) := (others => '0');		
	signal writedata	: std_logic_vector(31 downto 0);		
	signal wr			: std_logic;							
	signal rd			: std_logic;							

  constant clock_period: time := 20 ns;
  signal stop_the_clock: boolean := false;

begin

  UUT: lutForward port map (clk 		=> clk,
							clk_en		=> clk_en,
							reset		=> reset,
							start		=> start,
							dataa		=> dataa,
							datab		=> datab,
							n			=> n,
							waitrequest	=> waitrequest,
							done		=> done,
							result		=> result,
							addr		=> addr,
							readdata	=> readdata,
							writedata	=> writedata,
							wr			=> wr,
							rd			=> rd);

	clocking : process
	begin
		while not stop_the_clock loop
		  clk <= '0', '1' after clock_period / 2;
		  wait for clock_period;
		end loop;
		wait;
	end process clocking;
  
  
	stimulus: process
		begin
		wait for clock_period;
		  
		reset 	<= '0';
		n		<= '0';
		dataa	<= x"00001000";
		datab	<= x"00002000";
		wait for clock_period;
		  
		start 	<= '1';
		wait for clock_period;		-- Go through INIT
		  
		start 	<= '0';
		wait for clock_period;		-- Go through S0
		
		wait for clock_period;		-- Go through S1 and exit
		  
		
		start 	<= '1';
		n 		<= '1';
		dataa	<= x"00003000";
		datab	<= x"00080006";		-- n_neuron = x"0010" = d16 / LUT_size = x"0006" = d6 
		wait for clock_period;		-- Go through INIT
		  
		start 	<= '0';
		wait for clock_period;		-- Go through S0 
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		waitrequest	<= '1';
		wait for clock_period;		-- Go through S2
		
		wait for clock_period;		-- Go through S2 again because of the waitrequest
		
		waitrequest	<= '0';
		wait for clock_period;		-- Go through S2 again because of the waitrequest
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		waitrequest	<= '1';
		wait for clock_period;		-- Go through S3
		
		wait for clock_period;		-- Go through S3 again because of the waitrequest
		
		waitrequest	<= '0';
		wait for clock_period;		-- Go through S3 again because of the waitrequest
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		wait for clock_period;		-- Go through S4
		
		---------------------------------------------------------------------------------------------------------------
		
		wait for clock_period;		-- Go through S1
		
		wait for clock_period;		-- Go through S2
		
		readdata	<= x"12345678";
		wait for clock_period;		-- Go through S3
		
		readdata	<= x"87654321";
		waitrequest	<= '1';
		wait for clock_period;		-- Go through S4
		
		wait for clock_period;		-- Go through S4 again beacause of the waitrequest
		
		waitrequest	<= '0';
		wait for clock_period;		-- Go through S4 gain beacause of the waitrequest
		
		---------------------------------------------------------------------------------------------------------------

		stop_the_clock <= true;
		wait;
		
	end process stimulus;
end architecture;