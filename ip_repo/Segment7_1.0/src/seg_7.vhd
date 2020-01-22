library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


 
entity bcd_7segment is
Port 
( 
    BCDin : in STD_LOGIC_VECTOR (3 downto 0);
    Seven_Segment : out STD_LOGIC_VECTOR (7 downto 0)
);
end bcd_7segment;
 
architecture Behavioral of bcd_7segment is

constant A : std_logic_vector(7 downto 0):= "01000000";
constant B : std_logic_vector(7 downto 0):= "10000000";
constant C : std_logic_vector(7 downto 0):= "00010000";
constant D : std_logic_vector(7 downto 0):= "00100000";
constant E : std_logic_vector(7 downto 0):= "00000100";
constant F : std_logic_vector(7 downto 0):= "00001000";
constant G : std_logic_vector(7 downto 0):= "00000001";
constant DP : std_logic_vector(7 downto 0):= "00000010";

begin
 
process(BCDin)
begin
case BCDin is -- A B C D E F G
when "0000" =>
Seven_Segment <= A OR B OR C OR D OR E OR F; ---0
when "0001" => 
Seven_Segment <= B OR C; ---1
when "0010" => 
Seven_Segment <= A OR B OR G OR E OR D; ---2
when "0011" => 
Seven_Segment <= A OR B OR G OR C OR D; ---3
when "0100" => 
Seven_Segment <= F OR G OR B OR C; ---4
when "0101" => 
Seven_Segment <= A OR F OR G OR C OR D; ---5
when "0110" => 
Seven_Segment <= A OR F OR G OR E OR C OR D; ---6
when "0111" =>
Seven_Segment <= A OR B OR C; ---7
when "1000" => 
Seven_Segment <= A OR B OR C OR D OR E OR F OR G; ---8
when "1001" => 
Seven_Segment <= A OR B OR C OR D OR F OR G; ---9
when "1010" => 
Seven_Segment <= E OR F OR A OR B OR G OR C; ---puntje
when "1011" => 
Seven_Segment <= F OR G OR C OR D OR E;
when "1100" => 
Seven_Segment <= A OR F OR E OR D;
when "1101" => 
Seven_Segment <= B OR G OR E OR D OR C;
when "1110" => 
Seven_Segment <= A OR F OR G OR E OR D;
when "1111" => 
Seven_Segment <= A OR F OR G OR E;
when others =>
Seven_Segment <= DP; ---null
end case;
 
end process;
 
end Behavioral;