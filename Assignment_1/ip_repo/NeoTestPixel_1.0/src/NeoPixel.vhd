
----------------------------------------------------------------------------------
-- Company:     Hogeschool PXL
-- Engineer:    Vincent claes
-- 
-- Create Date: 27.11.2017 10:20:30
-- Design Name: 
-- Module Name: WS2812 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- by Vincent Claes
-- vincent.claes[at]pxl.be
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use ieee.std_logic_unsigned.all;

entity NeoPixel is
    Port ( 
        clk:    in  std_logic;
        d_out:  out std_logic
    );
end NeoPixel;

architecture arch of NeoPixel is

    constant T0_h   :   integer := 17;     --(0.34탎 / 0.35탎)
    constant T0_l   :   integer := 40;     --(/0.8탎)
    constant T1_h   :   integer := 35;     --(/0.7탎)
    constant T1_l   :   integer := 30;     --(/0.6탎)
    constant Reset  :   integer := 2800;   --(60탎 / 50탎)
    
    type LED_matrix is array(0 to 8)   of std_logic_vector(23 downto 0);
    type st_machine is (loading_state, sending_state, send_bit_state,reset_state);
    
begin

    process
        variable    state               : st_machine := loading_state;
        variable    GRB                 : std_logic_vector (23 downto 0)    := x"000000";
        variable    delay_high_cntr     : integer := 0;
        variable    delay_low_cntr      : integer := 0;
        variable    index               : integer := 0;
        variable    bit_cntr            : integer := 0;
        variable    LEDS                : LED_matrix    := (
                                                                x"FFFFFF",  -- LED 0, Green Red Blue
                                                                "111111110000000000000000",  -- LED 1
                                                                "000000001111111100000000",  -- LED 2
                                                                "000000000000000011111111",  -- LED 3
                                                                x"FFF000",  -- LED 4
                                                                x"000FFF",  -- LED 5
                                                                x"FF00FF",  -- LED 6
                                                                x"00FFFF",
                                                                x"FFFF00");  -- LED 63
        
        begin
            if rising_edge(clk) then
                    case state is
                        when loading_state  =>
                                GRB         := LEDS(index);
                                bit_cntr    := 24;
                                state       := sending_state;
                        when sending_state  =>
                                if( bit_cntr  > 0) then
                                    bit_cntr    := bit_cntr -1;
                                    if GRB(bit_cntr) ='1' then
                                        delay_high_cntr := T1_h;
                                        delay_low_cntr  := T1_l;
                                    else
                                        delay_high_cntr := T0_h;
                                        delay_low_cntr  := T0_l;
                                    end if;
                                    state   := send_bit_state;
                                else
                                    if( index < 63) then
                                        index   := index + 1;
                                        state   := loading_state;
                                    else
                                        delay_low_cntr := Reset;
                                        state          := reset_state;
                                    end if;
                                end if;
                             when send_bit_state =>
                                if (delay_high_cntr > 0) then
                                    d_out <= '1';
                                    delay_high_cntr := delay_high_cntr - 1;
                                elsif (delay_low_cntr > 0) then
                                    d_out <= '0';
                                    delay_low_cntr := delay_low_cntr - 1;
                                else
                                    state := sending_state;
                                end if;
                            when reset_state    =>
                                if (delay_low_cntr > 0) then
                                    d_out <= '0';
                                    delay_low_cntr := delay_low_cntr - 1;
                                else
                                    index := 0;
                                    state :=loading_state;
                                end if;
                            when others         =>  NULL;
                       end case;
                
            end if;
    end process;

end arch;

