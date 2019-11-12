#Pins for NEOPixel Matrix DOUT
set_property PACKAGE_PIN R11 [get_ports d_out_0]
#set_property PACKAGE_PIN R12 [get_ports d_out_1]
set_property IOSTANDARD LVCMOS33 [get_ports d_out_0]
#set_property IOSTANDARD LVCMOS33 [get_ports d_out_1]

#Pins for HCRS04
set_property IOSTANDARD LVCMOS33 [get_ports sonar_echo_0]
set_property IOSTANDARD LVCMOS33 [get_ports sonar_trig_0]
set_property PACKAGE_PIN L15 [get_ports sonar_echo_0]
set_property PACKAGE_PIN M15 [get_ports sonar_trig_0]

#Pins for GPIO SW input
set_property IOSTANDARD LVCMOS33 [get_ports GPIO_0_tri_i[0]]
set_property IOSTANDARD LVCMOS33 [get_ports GPIO_0_tri_i[1]]
set_property PACKAGE_PIN R12 [get_ports GPIO_0_tri_i[0]]
set_property PACKAGE_PIN R13 [get_ports GPIO_0_tri_i[1]]

#DEBUGGING PURPOSE
# To ARD_A3 on Arduino 6-pin  Pin 3 (and PL_LED Green)
#set_property PACKAGE_PIN E13 [get_ports ARD_ADDR3]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR3]
set_property PACKAGE_PIN E13 [get_ports led_g_0]
set_property IOSTANDARD LVCMOS33 [get_ports led_g_0]

# To ARD_A4 on Arduino 6-pin  Pin 2 (and PL_LED Red)
#set_property PACKAGE_PIN E12 [get_ports ARD_ADDR4]
#set_property IOSTANDARD LVCMOS33 [get_ports ARD_ADDR4]
set_property PACKAGE_PIN E12 [get_ports led_r_0]
set_property IOSTANDARD LVCMOS33 [get_ports led_r_0]