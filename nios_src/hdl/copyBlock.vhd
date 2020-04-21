library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use IEEE.std_logic_unsigned.all;


entity copyBlock is
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
end entity copyBlock;

architecture a_custominstruction of copyBlock is

	-- Internal signals
    signal new_clk: std_logic;
    signal i, j: integer;
	signal size, length, x, y: std_logic_vector(15 downto 0);
	signal target, source_array, temp: std_logic_vector(31 downto 0);
    
	-- States:
	type t_etat is (INIT, S1, S2, S3, S4); 
	signal etat : t_etat;
begin
	-- Combinatory logic
	new_clk <= clk AND clk_en;
	-- Custom Instruction state machine
	process( new_clk, reset )
	begin
		if( reset = '1') then
		elsif( rising_edge(new_clk) ) then
			case etat is 
		when INIT =>
                	done <= '0';
                    	if(start = '0') then
                    		etat <= INIT;
                   	else
                      		if(n="00") then
                          		size <= dataa(15 downto 0);
                          		target <= datab;
                          		done <= '1';
                          		etat <= INIT;
                      		elsif(n="01") then
                          		length <= dataa(15 downto 0);
                          		source_array <= datab;
                          		done <= '1';
                          		etat <= INIT;
                      		else
                          		x <= dataa(15 downto 0);
                          		y <= datab(15 downto 0);
                          		done <= '0'; rd <= '0'; wr <= '0'; addr <= (others => '0');
                          		wrdata <= (others => '0'); j <= 0; result <= (others => '0');
                          		etat <= S1;
                      		end if;
               	    	end if;
                when S1 =>
                	if (to_unsigned(j, 16) < unsigned(size)) then
                    		i <= 0;
                        	etat <= S2;
                    	else
                    		done <= '1';
                        	etat <= INIT;
                    	end if;
                when S2 =>
                	if(to_unsigned(i, 16) < unsigned(size)) then
                    		rd <= '1';
                        	addr <= std_logic_vector(unsigned(source_array) + ((unsigned(y)+to_unsigned(j, 16)) * unsigned(length) + (unsigned(x) + to_unsigned(i, 16))));
                        	etat <= S3;
                    	else
                    		j <= j + 1;
                        	etat <= S1;
                    	end if;
                when S3 =>
                	rd <= '0'; wr <= '1';
                    	if(unsigned(rddata) = X"000000FF") then
                      		wrdata <= X"00000001";
                    	else
                      		wrdata <= X"00000000";
                    	end if;
                    	addr <= std_logic_vector(unsigned(target) + (to_unsigned(j, 16)*unsigned(size) + to_unsigned(i, 16)));
                	if (busy = '0') then
                        	etat <= S4;
                    	else
                    		etat <= S3;
                    	end if;
                when S4 =>
                	rd <= '0'; wr <= '0';
                    	i <= i + 1;
                    	if(busy = '0') then
                     		etat <= S2;
                    	else
                    		etat <= S4;
                    	end if;
				when others => 
			end case;
		end if;	-- IF RST
	end process;
end architecture a_custominstruction;