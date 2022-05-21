-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xproko47
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK 	 		: in std_logic;
   RST 	 		: in std_logic;
	DIN 	 		: in std_logic;
	CCOUNT 		: in std_logic_vector(4 downto 0);
	DCOUNT 		: in std_logic_vector(3 downto 0);
	STATE_NUM 	: out std_logic_vector(2 downto 0)
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
-- Types and STATE_NUM both hold information about state so one is essentially redundant.
-- This is just to make creating this code easier while still staying truthful to the original draft.
type STATE_TYPE is (WAITING, BEGINS, DATA, ENDS, VALID); -- BEGIN and END are reserved names
signal state : STATE_TYPE := WAITING;
begin
	process (CLK, RST, CCOUNT, DCOUNT, state, DIN) begin
	if rising_edge(CLK) then
		if RST = '1' then
			state <= WAITING;
			STATE_NUM <= "000";
		else
			case state is
			when WAITING => if DIN = '0' then
									state <= BEGINS;
									STATE_NUM <= "001";
								 end if;
			when BEGINS => if CCOUNT = "10111" then
									state <= DATA;
									STATE_NUM <= "010";
							  end if;
			when DATA => if DCOUNT = "1000" then
								   state <= ENDS;
									STATE_NUM <= "011";
							  end if;
			when ENDS => if CCOUNT = "01111" and DIN = '1' then
									state <= VALID;
									STATE_NUM <= "100";
							  end if;
			when VALID => 
							state <= WAITING;
							STATE_NUM <= "000";
			when others			  => null;
			end case;
		end if;
	end if;
	end process;
end behavioral;
