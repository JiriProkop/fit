-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xditej01
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK              : in    std_logic;
   RST              : in    std_logic;
   DAT              : in    std_logic;
   ALL_DATA_READ    : in    std_logic;
   CLK_CNT          : in    std_logic_vector(4 downto 0);
   READ_EN          : out   std_logic;
   CLK_CNT_EN       : out   std_logic;
   VALID            : out   std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
type state is (WAIT_START, WAIT_BEGIN, DATA, WAIT_END, VALID_S);
signal myState : state := WAIT_START;
begin
    READ_EN <= '1' when myState = DATA else '0';
    VALID <= '1' when myState = VALID_S else '0';
    CLK_CNT_EN <= '0' when myState = VALID_S or myState = WAIT_START else '1';
    process (CLK) begin
        if rising_edge(CLK) then
            if RST = '1' then
                myState <= WAIT_START;
            else
                case myState is
                when WAIT_START => 
                    if DAT = '0' then
                        myState <= WAIT_BEGIN;
                    end if;
                when WAIT_BEGIN =>
                    if CLK_CNT = "10110" then
                        myState <= DATA;
                    end if;
                when DATA =>
                    if ALL_DATA_READ = '1' then
                        myState <= WAIT_END;
                    end if;
                when WAIT_END =>
                    if CLK_CNT = "10000" then
                        myState <= VALID_S;
                    end if;
                when VALID_S => myState <= WAIT_START;
                end case;
            end if;
        end if;
    end process;
end behavioral;
