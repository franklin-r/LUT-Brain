-- Code your testbench here
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use IEEE.std_logic_unsigned.all;

entity copyBlock_tb is
end entity copyBlock_tb;

architecture behavior of copyBlock_tb is

  component copyBlock is
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
      signal start  : in std_logic; -- Active high signal used to specify that inputs are valid
      signal done   : out std_logic; -- Active high signal used to notify the CPU that result is valid
      signal dataa  : in std_logic_vector(31 downto 0);  -- Operand A (always required)
      signal datab  : in std_logic_vector(31 downto 0);  -- Operand B (optional)
      signal result : out std_logic_vector(31 downto 0);	-- result (always required)
      signal n      : in std_logic_vector(1 downto 0)	   -- Operation extension - 4 modes support
  );
  end component;

	signal clk    : std_logic := '0';
	signal clk_en : std_logic;
	signal reset  : std_logic;
	signal addr     : std_logic_vector(31 downto 0);
	signal rd       : std_logic;
	signal wr 	    : std_logic;
	signal wrdata   : std_logic_vector(31 downto 0);
	signal rddata   : std_logic_vector(31 downto 0);
	signal busy     : std_logic;
	signal start  : std_logic;
	signal done   : std_logic;
	signal dataa  : std_logic_vector(31 downto 0);
	signal datab  : std_logic_vector(31 downto 0);
	signal result : std_logic_vector(31 downto 0);
    signal n      : std_logic_vector(1 downto 0);

constant clk_period : time := 20 ns;	
    
    BEGIN
    
    UUT : copyBlock 
    PORT MAP(
   	  clk => clk,
      clk_en => clk_en,
      reset => reset,
      addr => addr,
      rd => rd,
      wr => wr,
      wrdata => wrdata,
      rddata => rddata,
      busy => busy,
      start => start,
      done => done,
      dataa => dataa,
      datab => datab,
      result => result,
      n => n
    );

clk <= not clk after clk_period/2;
    
    copyBlock_proc: process
    begin
		busy  <= '0';  --Avalon bus never busy
		clk_en <= '1'; --Clock always enabled
      	rddata <= x"0000_00FF";   --Avalon Readdata bus /= 0 so all states are tested
		  
		--Reset system
		reset <= '1';
		wait for 10 ns;
      	reset <= '0';
		
	    start <= '1';  --Custom instructions start signal asserted
		
      	--------- INIT ---------
		--Write parameters 
		n <= "00";
		dataa <= X"0000_0003";-- size
		datab <= X"0000_FF00";-- target
		wait for clk_period;
      
		
		n <= "01";
		dataa <= X"0000_0004";-- length
		datab <= X"0000_00FF";-- source_array
		wait for clk_period;
      
		--Start work
		n <= "10";
		dataa <= X"0000_0000";-- x
        datab <= X"0000_0000";-- y
		wait for clk_period;     
		
		start <= '1';  --Custom instructions start signal deasserted
		start <= '0';
        
      	---------S1---------
        
        wait for clk_period;
      	---------S2---------
        
        wait for clk_period;
        ---------S3---------
        
        wait for clk_period;
        ---------S4---------
        rddata <= x"0000_0000";
        wait for clk_period;
		wait;
		
   end process copyBlock_proc;
end behavior;
    