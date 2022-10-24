-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2022 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): jmeno <login AT stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
   );
   end cpu;
   
   
   -- ----------------------------------------------------------------------------
   --                      Architecture declaration
   -- ----------------------------------------------------------------------------
   architecture behavioral of cpu is

	type fsm_state is (sidle, sstart, swhatever);

	signal pstate : fsm_state;
	signal nstate : fsm_state;

	-- CNT
	signal cnt_reg : std_logic_vector (11 downto 0);
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;
	--

	-- PTR
	signal ptr_reg : std_logic_vector (9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	--

	-- MUX
	signal mux_sel : std_logic_vector (1 downto 0);
	--

	-- PC
	signal pc_reg : std_logic_vector(15 downto 0);
	signal pc_set : std_logic_vector(15 downto 0);
	signal pc_ld : std_logic;
	signal pc_inc : std_logic;
	-------------------------------------------------------------------------------
begin
  -- PC
  program_counter: process (RESET, CLK)
  begin
    if (RESET='1') then
      pc_reg <= (others=>'0');
    elsif rising_edge(CLK) then
      if (pc_ld='1') then
        pc_reg <= pc_set;
      elsif (pc_inc='1') then
        pc_reg <= pc_reg + 1;
      end if;
    end if;
  end process;
  --

  -- PTR
	pointer: process (RESET, CLK)
	begin
		if (RESET = '1') then
			ptr_reg <= (others => '0');
		elsif (rising_edge(CLK)) then
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec = '1') then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	DATA_ADDR <= ptr_reg;
	--

	-- CNT
	counter: process (RESET, CLK)
	begin
		if (RESET = '1') then
			cnt_reg <= (others => '0');
		elsif (rising_edge(CLK)) then
			if (cnt_inc = '1') then
				cnt_reg <= cnt_reg + 1;
			elsif (cnt_dec = '1') then
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;
	--

	-- MUX
	mux: process (RESET, CLK)
	begin
		if (RESET = '1') then
			DATA_WDATA <= (others => '0');	
		elsif (rising_edge(CLK)) then
			if (mux_sel = "00") then
				DATA_WDATA <= IN_DATA;
			elsif (mux_sel = "01") then
				DATA_WDATA <= DATA_RDATA + 1;
			elsif (mux_sel = "10") then
				DATA_WDATA <= DATA_RDATA - 1;
			else 
				DATA_WDATA <= (others => '0');
			end if;
		end if;
	end process;
	--
	-- FSM state register
	fsm_state_process: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pstate <= sidle;
		elsif (rising_edge(CLK)) then
			if (EN = '1') then
				pstate <= nstate;
			end if;
		end if;
	end process;
  --

  -- FSM
  fsm: process (pstate, IN_VLD, OUT_BUSY, DATA_RDATA, cnt_reg)
  begin
	DATA_EN <= '0';
	DATA_RDWR <= '0';
	IN_REQ <= '0';
	OUT_WE <= '0';

	pc_inc <= '0';
	pc_ld <= '0';

	cnt_inc <= '0';
	cnt_dec <= '0';
	ptr_inc <= '0';
	ptr_dec <= '0';
  end process;

end behavioral;
