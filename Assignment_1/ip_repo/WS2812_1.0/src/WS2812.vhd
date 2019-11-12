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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity WS2812 is
    generic(
        clock_frequency: integer := 50_000_000 -- Hertz
    );
    Port ( 
        clk:    in  std_logic;
        LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8, LED_9, 
        LED_10,LED_11, LED_12, LED_13, LED_14,LED_15, LED_16, LED_17, LED_18, LED_19,
        LED_20,LED_21,LED_22,LED_23,LED_24,LED_25,LED_26,LED_27,LED_28,LED_29,
        LED_30,LED_31,LED_32,LED_33,LED_34,LED_35,LED_36,LED_37,LED_38,LED_39,
        LED_40,LED_41,LED_42,LED_43,LED_44,LED_45,LED_46,LED_47,LED_48,LED_49,
        LED_50,LED_51,LED_52,LED_53,LED_54,LED_55,LED_56,LED_57,LED_58,LED_59,
        LED_60,LED_61,LED_62,LED_63 : IN std_logic_vector(23 downto 0);
        d_out:  out std_logic;
        led_r: out std_logic;
        led_g: out std_logic;
        led_b: out std_logic
    );
end WS2812;

architecture Behavioral of WS2812 is

    constant T0_h   :   integer := 17;
    constant T0_l   :   integer := 38;
    constant T1_h   :   integer := 35;
    constant T1_l   :   integer := 28;
    constant Reset  :   integer := 2500;
    
    type LED_matrix is array(0 to 63)   of std_logic_vector(23 downto 0);
    type st_machine is (loading_state, sending_state, send_bit_state,reset_state);


begin
    process
        variable    state               : st_machine := loading_state;
        variable    GRB                 : std_logic_vector (23 downto 0)    := x"000000";
        variable    delay_high_cntr     : integer := 0;
        variable    delay_low_cntr      : integer := 0;
        variable    index               : integer := 0;
        variable    bit_cntr            : integer := 0;
        --variable    test                :
        variable    LEDS                : LED_matrix;
begin
    wait until rising_edge(clk);
    --Green Red Blue
        LEDS := ( LED_0, LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7, LED_8, LED_9, 
        LED_10,LED_11, LED_12, LED_13, LED_14,LED_15, LED_16, LED_17, LED_18, LED_19,
        LED_20,LED_21,LED_22,LED_23,LED_24,LED_25,LED_26,LED_27,LED_28,LED_29,
        LED_30,LED_31,LED_32,LED_33,LED_34,LED_35,LED_36,LED_37,LED_38,LED_39,
        LED_40,LED_41,LED_42,LED_43,LED_44,LED_45,LED_46,LED_47,LED_48,LED_49,
        LED_50,LED_51,LED_52,LED_53,LED_54,LED_55,LED_56,LED_57,LED_58,LED_59,
        LED_60,LED_61,LED_62,LED_63); 
    
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
                                    state := loading_state;
                                end if;
        when others         => 
                                NULL;
    end case;
    
    led_r <= LED_0(9);
    led_g <= LED_0(17);
    led_b <= LED_0(0);
    
 end process;
end Behavioral;
