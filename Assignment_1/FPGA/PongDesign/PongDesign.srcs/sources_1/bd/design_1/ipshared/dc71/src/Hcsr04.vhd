library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity Hcsr04 is
    Port ( clk        : in  STD_LOGIC;
           sonar_trig : out STD_LOGIC;
           sonar_echo : in  STD_LOGIC;
		   ReadValue  : out  STD_LOGIC_VECTOR(15 downto 0));
end Hcsr04;

architecture Behavioral of Hcsr04 is
	signal waarde : unsigned(15 downto 0) := (others => '0'); 
    signal count            : unsigned(16 downto 0) := (others => '0');
    signal echo_last        : std_logic := '0';
    signal echo_synced      : std_logic := '0';
    signal echo_unsynced    : std_logic := '0';
    signal waiting          : std_logic := '0'; 
 --   signal seven_seg_count  : unsigned(15 downto 0) := (others => '0');
begin


    
process(clk)
    begin
        if rising_edge(clk) then
            if waiting = '0' then
                if count = 1000 then -- Assumes 100MHz
                   -- After 10us then go into waiting mode
                   sonar_trig <= '0';
                   waiting    <= '1';
                   count       <= (others => '0');
                else
                   sonar_trig <= '1';
                   count <= count+1;
                end if;
            elsif echo_last = '0' and echo_synced = '1' then
                -- Seen rising edge - start count
                count       <= (others => '0');
                waarde <= (others => '0');
                
            elsif echo_last = '1' and echo_synced = '0' then
                -- Seen falling edge, so capture count
				ReadValue <= std_logic_vector(waarde);

            elsif count = 2900*2 -1 then
                -- advance the counter
				waarde <= waarde +1;               
                count <= (others => '0');
                if waarde = 3448 then
                    -- time out - send another pulse
                    waiting <= '0';
                end if;
            else
                count <= count + 1;                
            end if;
            echo_last     <= echo_synced;
            echo_synced   <= echo_unsynced;
            echo_unsynced <= sonar_echo;
        end if;
        
    end process;
    
end Behavioral;
