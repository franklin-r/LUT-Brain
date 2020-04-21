LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;

--Entity declaration
ENTITY buildAddress_tb IS
END buildAddress_tb ;

ARCHITECTURE tb OF buildAddress_tb IS 

	-- Component Declaration for the Unit Under Test (UUT)
	component buildAddress is
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
	end component;    
	
	--signal declaration
	signal clk    : std_logic := '0';  --Initialize clock 
	signal clk_en : std_logic;	 
	signal reset  : std_logic;	 
	signal addr   : std_logic_vector(31 downto 0);
	signal rd     : std_logic;
	signal wr 	  : std_logic;
	signal wrdata : std_logic_vector(31 downto 0);
	signal rddata : std_logic_vector(31 downto 0);
	signal busy   : std_logic;
	signal start  : std_logic;
	signal done   : std_logic; 
	signal dataa  : std_logic_vector(31 downto 0);  
	signal datab  : std_logic_vector(31 downto 0);  
	signal result : std_logic_vector(31 downto 0);	
	signal n      : std_logic_vector(1 downto 0);   

BEGIN

	--Component instanciation
    U0_buildAddress : buildAddress
    port map    (	
	     clk    => clk    ,
	     clk_en => clk_en ,
	     reset  => reset  ,
	     addr   => addr   ,
	     rd     => rd     ,
	     wr 	   => wr 	 ,
	     wrdata => wrdata ,
	     rddata => rddata ,
	     busy   => busy   ,
	     start  => start  ,
	     done   => done   ,
	     dataa  => dataa  ,
	     datab  => datab  ,
	     result => result ,
	     n      => n
	);	
	
    clk <= not clk after 10 ns;  

	 do_check_out_result:process
    begin
		busy  <= '0';  --Avalon bus never busy
		clk_en <= '1'; --Clock always enabled
      rddata <= x"0000_0000";  
		  
		--Reset system
		reset <= '1';
		wait for 10 ns;
      reset <= '0';
		
	   start <= '1';  --Custom instructions start signal asserted
		
		--Write parameters 
		n <= "00";
		dataa <= (2 => '1', others => '0'); --4 neurons
		datab <= (2 downto 1 => '1', others => '0'); --6 inputs per neurons
		wait for 40 ns;
      
		
		n <= "01";
		dataa <= x"0000_00FF"; --Source address 
		datab <= x"0000_FF00"; --LUT address
		wait for 40 ns;
      
		--Start work
		n <= "10";
		datab <= x"00FF_0000"; --Currrent position address
		wait for 40 ns;     
		
		start <= '1';  --Custom instructions start signal deasserted
		start <= '0';
		wait for 200 ns;
		
		rddata <= x"0000_0001"; 
		wait for 5 us;
		
   end process do_check_out_result;
end tb;