-- uart.vhd: UART controller - receiving part
-- Author(s): xproko47
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
  CLK: 	    in std_logic;
	RST: 	    in std_logic;
	DIN: 	    in std_logic;
	DOUT: 	    out std_logic_vector(7 downto 0);
	DOUT_VLD: 	out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
signal ccnt  		  : std_logic_vector(4 downto 0);
signal dcnt  		  : std_logic_vector(3 downto 0);
signal state_num	: std_logic_vector(2 downto 0);
begin
	FSM: entity work.UART_FSM(behavioral)
	port map(
		CLK			=> CLK,
		RST			=> RST,
		DIN			=> DIN,
		CCOUNT			=> ccnt,
		DCOUNT			=> dcnt,
		STATE_NUM	=> state_num
	);
	process (CLK, RST, DIN, ccnt, dcnt, state_num) begin
		if rising_edge(CLK) then
			if RST = '1' then
				ccnt <= "00000";
				dcnt <= "0000";
			else
				if state_num = "000" then
					DOUT_VLD <= '0';
					ccnt <= "00000";
					dcnt <= "0000";
				end if;
				if state_num = "001" then
					ccnt <= ccnt + 1;
				end if;
				if state_num = "010" then
					if dcnt = "1000" then
						ccnt <= "00000";
						DOUT(7) <= DIN;
					elsif ccnt = "11000" or ccnt = "10000" then
						dcnt <= dcnt + 1;
						ccnt <= "00000";
						case dcnt is
						when "0001" => DOUT(0) <= DIN;
						when "0010" => DOUT(1) <= DIN;
						when "0011" => DOUT(2) <= DIN;
						when "0100" => DOUT(3) <= DIN;
						when "0101" => DOUT(4) <= DIN;
						when "0110" => DOUT(5) <= DIN;
						when "0111" => DOUT(6) <= DIN;
						when others			  => null;
						end case;
					else
						ccnt <= ccnt + 1;
					end if;
				end if;
				if state_num = "011" then
					if ccnt = "10000" then
						ccnt <= "00000";
					else
						ccnt <= ccnt + 1;
					end if;
				end if;
				if state_num = "100" then
					DOUT_VLD <= '1';
				end if;
			end if;
		end if;
	end process;
end behavioral;
