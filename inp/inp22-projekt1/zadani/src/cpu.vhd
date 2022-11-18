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

	type fsm_state is (
		s_idle, 
		s_fetch, 
		s_prefetch, 
		s_decode, 

		s_ptr_inc, 
		s_ptr_dec, 

		s_val_inc0, 
		s_val_inc1, 
		s_val_inc2,

		s_val_dec0,
		s_val_dec1,
		s_val_dec2,

		s_print0,
		s_print1,
		s_print2,

		s_get0,
		s_get1,

		-- while loop
		s_while0,
		s_while1,
		s_while2,
		s_while3,
		s_while4,
		s_while5,
		s_while6,
		s_while7,
		s_while8,
		s_while9,
		s_whileA,
		s_whileB,
		s_whileC,
		s_whileD,
		s_whileE,
		s_whileF,
		s_while10,
		s_while11,

		--do-while loop
		s_dowhile0,
		s_dowhile1,
		s_dowhile2,
		s_dowhile3,
		s_dowhile4,
		s_dowhile5,
		s_dowhile6,
		s_dowhile7,
		s_dowhile8,
		s_dowhile9,
		s_dowhileA,
		s_dowhileB,

		s_null);

	signal pstate : fsm_state;
	signal nstate : fsm_state;

	-- CNT
	signal cnt_reg : std_logic_vector (11 downto 0);
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;
	--

	-- PTR
	signal ptr_reg : std_logic_vector (11 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	--

	-- MUX1
	signal mux1_sel : std_logic;
	signal mux2_sel : std_logic_vector (1 downto 0);
	--

	-- PC
	signal pc_reg : std_logic_vector(11 downto 0);
	signal pc_dec : std_logic;
	signal pc_inc : std_logic;
	-------------------------------------------------------------------------------
begin
  -- PC
  program_counter: process (RESET, CLK)
  begin
    if (RESET = '1') then
      pc_reg <= (others => '0');
    elsif rising_edge(CLK) then
      if (pc_dec = '1') then
        pc_reg <= pc_reg - 1;
      elsif (pc_inc = '1') then
        pc_reg <= pc_reg + 1;
      end if;
    end if;
  end process;
  --

  -- PTR
	pointer: process (RESET, CLK)
	begin
		if (RESET = '1') then
			ptr_reg <= "000000000000";
		elsif (rising_edge(CLK)) then
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec = '1') then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
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

	-- MUX1
	mux1: process (RESET, CLK)
	begin
		if (rising_edge(CLK)) then
			if (mux1_sel = '0') then --nacitam kod
				DATA_ADDR <= '0' & pc_reg;
			elsif (mux1_sel = '1') then --nacitam data
				DATA_ADDR <= '1' & ptr_reg;
			end if;
		end if;
	end process;
	--

	-- MUX2
	mux2: process (RESET, CLK)
	begin
		if (RESET = '1') then
			DATA_WDATA <= (others => '0');	
		elsif (rising_edge(CLK)) then
			if (mux2_sel = "00") then
				NULL;
			elsif (mux2_sel = "01") then
				DATA_WDATA <= DATA_RDATA + 1;
			elsif (mux2_sel = "10") then
				DATA_WDATA <= DATA_RDATA - 1;
			elsif (mux2_sel = "11") then
				DATA_WDATA <= IN_DATA;
			end if;
		end if;
	end process;
	
	-- FSM state register
	fsm_pstate_process: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pstate <= s_idle;
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
	
	OUT_WE <= '0';

	cnt_inc <= '0';
	cnt_dec <= '0';
	IN_REQ <= '0';
	ptr_inc <= '0';
	ptr_dec <= '0';
	pc_inc <= '0';
	pc_dec <= '0';

	mux1_sel <= '0';
	mux2_sel <= "00";

	case pstate is
		when s_idle =>
			nstate <= s_prefetch;
		when s_prefetch =>
			nstate <= s_fetch;
		when s_fetch =>
			DATA_EN <= '1';
			nstate <= s_decode;
		when s_decode =>
			case DATA_RDATA is
				when X"3E" =>
					nstate <= s_ptr_inc;
				when X"3C" =>
					nstate <= s_ptr_dec;
				when X"2B" =>
					mux1_sel <= '1';
					nstate <= s_val_inc0;
				when X"2D" =>
					mux1_sel <= '1';
					nstate <= s_val_dec0;
				when X"2E" =>
					mux1_sel <= '1';
					nstate <= s_print0;
				when X"2C" =>
					IN_REQ <= '1';
					nstate <= s_get0;
				when X"5B" =>
					pc_inc <= '1';
					mux1_sel <= '1';
					nstate <= s_while0;
				when X"5D" =>
					mux1_sel <= '1';
					nstate <= s_while6;
				when X"28" =>
					pc_inc <= '1';
					nstate <= s_prefetch;
				when X"29" =>
					mux1_sel <= '1';
					nstate <= s_dowhile0;
				when X"00" =>
					nstate <= s_null;
				when others =>
					pc_inc <= '1';
					nstate <= s_prefetch;
			end case;
		when s_null =>
			nstate <= s_null;
		-- >
		when s_ptr_inc =>
			ptr_inc <= '1';
			pc_inc <= '1';
			nstate <= s_prefetch;
		-- <
		when s_ptr_dec =>
			ptr_dec <= '1';
			pc_inc <= '1';
			nstate <= s_prefetch;
		-- +
		when s_val_inc0 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_val_inc1;
		when s_val_inc1 =>
			mux2_sel <= "01";
			mux1_sel <= '1';
			nstate <= s_val_inc2;
		when s_val_inc2 =>
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			pc_inc <= '1';
			nstate <= s_prefetch;
		-- -
		when s_val_dec0 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_val_dec1;
		when s_val_dec1 =>
			mux2_sel <= "10";
			mux1_sel <= '1';
			nstate <= s_val_dec2;
		when s_val_dec2 =>
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			pc_inc <= '1';
			nstate <= s_prefetch;
		-- .
		when s_print0 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_print1;
		when s_print1 =>
			if (OUT_BUSY = '1') then
				nstate <= s_print1;
			else
				nstate <= s_print2;
			end if;
		when s_print2 =>
			OUT_WE <= '1';
			OUT_DATA <= DATA_RDATA;
			pc_inc <= '1';
			nstate <= s_prefetch;
		-- ,
		when s_get0 =>
			if (IN_VLD = '1') then
				nstate <= s_get1;
				mux2_sel <= "11";
				mux1_sel <= '1';
				DATA_EN <= '1';
			else
				IN_REQ <= '1';
				nstate <= s_get0;
			end if;
		when s_get1 =>
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			pc_inc <= '1';
			nstate <= s_prefetch;
		-- [
		when s_while0 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_while1;
		when s_while1 =>
			mux1_sel <= '1';
			nstate <= s_while2;
		when s_while2 =>
			if(DATA_RDATA = "00000000") then
				cnt_inc <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				nstate <= s_while3;
			else
				nstate <= s_prefetch;
			end if;
		when s_while3 =>
			nstate <= s_while4;
		when s_while4 =>
			if(cnt_reg = "000000000000") then
				nstate <= s_prefetch;
			else
				if (DATA_RDATA = X"5B") then -- if(DATA_RDATA == '[')
					cnt_inc <= '1';
				elsif (DATA_RDATA = X"5D") then -- if(DATA_RDATA == ']')
					cnt_dec <= '1';
				end if;
				pc_inc <= '1';
				nstate <= s_while5;
			end if;
		when s_while5 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_while3;
		-- ]
		when s_while6 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_while7;
		when s_while7 =>
			nstate <= s_while8;
		when s_while8 =>
			if (DATA_RDATA = "00000000") then
				pc_inc <= '1';
				nstate <= s_prefetch;
			else
				cnt_inc <= '1';
				pc_dec <= '1';
				nstate <= s_while9;
			end if;
		when s_while9 =>
			nstate <= s_whileA;
		when s_whileA =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_whileB;
		when s_whileB =>
			nstate <= s_whileC;
		when s_whileC =>	-- while(CNT != 0)
			if (cnt_reg = "000000000000") then
				nstate <= s_prefetch;
			else
				if (DATA_RDATA = X"5D") then -- if(DATA_RDATA == ']')
					cnt_inc <= '1';
				elsif (DATA_RDATA = X"5B") then -- if(DATA_RDATA == '[')
					cnt_dec <= '1';
				end if;
				nstate <= s_whileD;
			end if;
		when s_whileD =>
			nstate <= s_whileE;
		when s_whileE =>
			if (cnt_reg = "000000000000") then
				pc_inc <= '1';
			else
				pc_dec <= '1';
			end if;
			nstate <= s_whileF;
		when s_whileF =>
			nstate <= s_while10;
		when s_while10 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_while11;
		when s_while11 =>
			nstate <= s_whileC;

		-- )
		when s_dowhile0 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_dowhile1;
		when s_dowhile1 =>
			nstate <= s_dowhile2;
		when s_dowhile2 =>
			if (DATA_RDATA = "00000000") then
				pc_inc <= '1';
				nstate <= s_prefetch;
			else
				cnt_inc <= '1';
				pc_dec <= '1';
				nstate <= s_dowhile3;
			end if;
		when s_dowhile3 =>
			nstate <= s_dowhile4;
		when s_dowhile4 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_dowhile5;
		when s_dowhile5 =>
			nstate <= s_dowhile6;
		when s_dowhile6 =>	-- while(CNT != 0)
			if (cnt_reg = "000000000000") then
				nstate <= s_prefetch;
			else
				if (DATA_RDATA = X"29") then -- if(DATA_RDATA == ')')
					cnt_inc <= '1';
				elsif (DATA_RDATA = X"28") then -- if(DATA_RDATA == '(')
					cnt_dec <= '1';
				end if;
				nstate <= s_dowhile7;
			end if;
		when s_dowhile7 =>
			nstate <= s_dowhile8;
		when s_dowhile8 =>
			if (cnt_reg = "000000000000") then
				pc_inc <= '1';
			else
				pc_dec <= '1';
			end if;
			nstate <= s_dowhile9;
		when s_dowhile9 =>
			nstate <= s_dowhileA;
		when s_dowhileA =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			nstate <= s_dowhileB;
		when s_dowhileB =>
			nstate <= s_dowhile6;
		end case;
  end process;

end behavioral;