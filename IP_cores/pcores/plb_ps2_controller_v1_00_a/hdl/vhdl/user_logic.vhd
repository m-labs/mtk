-- PLB PS/2 controller (PLB v4.6)
--
-- Brief  : Handle PS/2 mice and keyboards
-- Author : Matthias Alles
-- Date   : 2005-2008
--
-- Used to initialize PS/2 mice and read data from keyboard and mouse.
-- Interrupts are generated in case something happened.

--
-- Copyright (C) Genode Labs, Feske & Helmuth Systementwicklung GbR
--               http://www.genode-labs.com
--
-- This file is part of the Genode-FX package, which is distributed
-- under the terms of the GNU General Public License version 2.
--

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v2_00_a;
use proc_common_v2_00_a.proc_common_pkg.all;

-- DO NOT EDIT ABOVE THIS LINE --------------------

--USER libraries added here

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--   C_NUM_REG                    -- Number of software accessible registers
--   C_NUM_INTR                   -- Number of interrupt event
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Reset                 -- Bus to IP reset
--   Bus2IP_Data                  -- Bus to IP data bus
--   Bus2IP_BE                    -- Bus to IP byte enables
--   Bus2IP_RdCE                  -- Bus to IP read chip enable
--   Bus2IP_WrCE                  -- Bus to IP write chip enable
--   IP2Bus_Data                  -- IP to Bus data bus
--   IP2Bus_RdAck                 -- IP to Bus read transfer acknowledgement
--   IP2Bus_WrAck                 -- IP to Bus write transfer acknowledgement
--   IP2Bus_Error                 -- IP to Bus error response
--   IP2Bus_IntrEvent             -- IP to Bus interrupt event
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    --USER generics added here
    C_CLK_IN                       : integer              := 100;
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_SLV_DWIDTH                   : integer              := 32;
    C_NUM_REG                      : integer              := 2;
    C_NUM_INTR                     : integer              := 2
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    key_data     : in  std_logic;
    key_clk      : in  std_logic;
    mouse_clk_I  : in  std_logic;
    mouse_clk_O  : out std_logic;
    mouse_clk_T  : out std_logic;
    mouse_data_I : in  std_logic;
    mouse_data_O : out std_logic;
    mouse_data_T : out std_logic;
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Reset                   : in  std_logic;
    Bus2IP_Data                    : in  std_logic_vector(0 to C_SLV_DWIDTH-1);
    Bus2IP_BE                      : in  std_logic_vector(0 to C_SLV_DWIDTH/8-1);
    Bus2IP_RdCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    Bus2IP_WrCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    IP2Bus_Data                    : out std_logic_vector(0 to C_SLV_DWIDTH-1);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic;
    IP2Bus_IntrEvent               : out std_logic_vector(0 to C_NUM_INTR-1)
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute SIGIS : string;
  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Reset  : signal is "RST";

end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

	signal interrupt	: std_logic_vector(0 to C_NUM_INTR-1);
	-- this register contains the ps2-data we received and is used by the driver
	type register32 is array(0 to C_NUM_REG-1) of std_logic_vector(0 to 31);
	signal ps2_data_reg: register32;

 	-- mouse init
	type mouse_init_state is (CLEAR_CLK, RELEASE_CLK, INIT_START, SEND, WAIT_ACK, RECEIVE, DONE);

	signal mouse_state: mouse_init_state;
	signal dir: std_logic;
	signal mouse_data_out: std_logic;  
	constant mouse_init: std_logic_vector(10 downto 0) :="11011110100"; 
	signal ptr: integer range 0 to 10;
	signal mouse_init_vec: std_logic_vector(32 downto 0);

	-- handling of ps2-keyboard
	signal kb_strobe   : std_logic;
	signal ms_strobe   : std_logic;
	signal break       : std_logic;
	signal break_saved : std_logic; -- um zu merken, ob breakcode
	signal data        : std_logic_vector(7 downto 0);  
	signal ms_data     : std_logic_vector(32 downto 0);

	signal kb_clk_t: std_logic;
	signal kb_clk_t2: std_logic;

	signal input_vector: std_logic_vector(10 downto 0);

	signal ms_clk_t: std_logic;
	signal ms_clk_t2: std_logic;

	signal input_vector_mouse: std_logic_vector(32 downto 0);

	signal mouse_init_counter: unsigned(13 downto 0);
	signal init_done: std_logic;

begin

	data      <= input_vector(8 downto 1); -- concurrent
	kb_strobe <= not input_vector(0);
	break     <= '1' when input_vector(8 downto 1) = "11110000" else '0';
	ms_strobe <= not input_vector_mouse(0);
	ms_data   <= input_vector_mouse;


	MASTER_REG_READ_PROC : process( Bus2IP_RdCE, ps2_data_reg ) is
	begin
		for i in 0 to C_NUM_REG-1 loop
			if Bus2IP_RdCE(i) = '1' then
				ip2bus_data <= ps2_data_reg(i);
			end if;
		end loop;
	end process master_reg_read_proc;


	-- read the data from the keyboard
	handle_keyboard: process(Bus2IP_Clk) is
	begin
		if Bus2IP_Clk = '1' and Bus2IP_Clk'event then
			if Bus2IP_Reset = '1' then
				input_vector <= (others => '1');
				kb_clk_t     <= '0';
				kb_clk_t2    <= '0';
			else
				kb_clk_t <= key_clk;
				kb_clk_t2 <= kb_clk_t;
				if kb_clk_t2 = '1' and kb_clk_t = '0' then  -- falling edge detected
					input_vector(9 downto 0) <= input_vector(10 downto 1);
					input_vector(10)         <= key_data;
				end if;

				-- when a whole byte was received clear the input vector
				if input_vector(0) = '0' then
					input_vector(10 downto 0) <= (others => '1');
				end if;
			end if;
		end if;
	end process handle_keyboard;

	-- read data from the mouse
	handle_mouse: process(Bus2IP_Clk) is
	begin
		if Bus2IP_Clk = '1' and Bus2IP_Clk'event then
			if Bus2IP_Reset = '1' then
				input_vector_mouse <= (others => '1');
				ms_clk_t     <= '0';
				ms_clk_t2    <= '0';
			else
				ms_clk_t  <= mouse_clk_i;
				ms_clk_t2 <= ms_clk_t;
				if ms_clk_t2 = '1' and ms_clk_t = '0' and init_done = '1' then  -- falling edge detected
					input_vector_mouse(31 downto 0) <= input_vector_mouse(32 downto 1);
					input_vector_mouse(32)          <= mouse_data_i;
				end if;

				if input_vector_mouse(0) = '0' then
					input_vector_mouse(32 downto 0) <= (others => '1');
				end if;
			end if;
		end if;

	end process handle_mouse;


	-- generate interrupt when a new data was received by keyboard
	keyboard_top: process(Bus2IP_Clk) is 
	begin
		if Bus2IP_Clk = '1' and Bus2IP_Clk'event then
			if Bus2IP_Reset = '1' then
				break_saved <= '0';
				interrupt(0) <= '0';
				ps2_data_reg(0) <= (others => '0');
			else
				interrupt(0) <= '0';
				if kb_strobe = '1' then
					if break = '0' then
						ps2_data_reg(0) <= "0000000000000000" & "00000000" & data;
						interrupt(0) <= '1';
					end if;
					break_saved <= break;
					if break_saved = '1' then
						ps2_data_reg(0) <= "0000000000000000" & "00000001" & data;
						interrupt(0) <= '1';
					end if;
				end if;
			end if;
		end if;
	end process keyboard_top;


	-- generate interrupt when new data was received by mouse
	mouse_top: process(Bus2IP_Clk) is 
	begin
		if Bus2IP_Clk = '1' and Bus2IP_Clk'event then
			if Bus2IP_Reset = '1' then
				interrupt(1)    <= '0';
				ps2_data_reg(1) <= (others => '0');
			else
				interrupt(1) <= '0';
				if ms_strobe = '1' then
					ps2_data_reg(1) <= "00000000" &  ms_data(30 downto 23) & ms_data(19 downto 12) & ms_data(8 downto 1);
					interrupt(1) <= '1';
				end if;
			end if;
		end if;
	end process mouse_top;


	-- this process has to initialize the mouse
	init_mouse: process(Bus2IP_Clk) is
	begin
		if Bus2IP_Clk = '1' and Bus2IP_Clk'event then
			if Bus2IP_Reset = '1' then
				mouse_data_t <= '0'; --drive when starting;
				mouse_data_o <= '0';
				mouse_state  <= CLEAR_CLK;
				mouse_init_vec <= (others => '1');
				ptr          <= 0;
				init_done    <= '0';
				mouse_clk_t  <= '0';
				mouse_clk_o  <= '0';
				mouse_init_counter <= (others => '0');
			else
				case mouse_state is
					when CLEAR_CLK  =>
						mouse_clk_t <= '0';  -- drive clock signal for 100us low
						mouse_clk_o <= '0';
						mouse_data_t <= '1';  -- don't drive data line
						mouse_init_counter <= mouse_init_counter + 1;
						if mouse_init_counter = C_CLK_IN*100 then -- wait 100 us
							mouse_state <= RELEASE_CLK;
							mouse_init_counter <= (others => '0');
							mouse_data_o <= '0';
							mouse_data_t <= '0'; -- drive data low too 
						end if;
					when RELEASE_CLK =>
						mouse_init_counter <= mouse_init_counter + 1; --wait some time
						if mouse_init_counter = C_CLK_IN*3 then
							mouse_clk_t      <= '1'; --release clock signal and wait for falling edge (in next state)
							mouse_state      <= SEND;
						end if;
					when INIT_START =>
						if ms_clk_t2 = '1' and ms_clk_t = '0' then  -- falling edge detected, mouse receives with rising edge, startbit
							mouse_state    <= SEND;
						end if;     
					when SEND =>
						if ms_clk_t2 = '1' and ms_clk_t = '0' then  -- falling edge detected
							mouse_data_o   <= mouse_init(ptr);
							ptr            <= ptr + 1;
							if ptr = 9 then
								mouse_data_t <= '1';  --release data_line
								mouse_state <= WAIT_ACK;
								ptr <= 0;
							end if;
						end if;
					when WAIT_ACK =>
						if ms_clk_t2 = '1' and ms_clk_t = '0' then  -- falling edge detected
							if mouse_data_i = '0' then  -- acknowledge by mouse
								mouse_state <= RECEIVE;
							end if;
						end if;
					when RECEIVE =>
						if ms_clk_t2 = '1' and ms_clk_t = '0' then  -- falling edge detected, we receive with falling edge
							mouse_init_vec(9 downto 0) <= mouse_init_vec(10 downto 1);
							mouse_init_vec(10) <= mouse_data_I;
						end if;
						if mouse_init_vec(0) = '0' then
							mouse_state <= DONE;
							mouse_init_vec(32 downto 0) <= (others => '1');
						end if;             
					when DONE   =>
						init_done <= '1';
					when others =>
				end case; 
			end if;
		end if;
	end process init_mouse;

	IP2Bus_IntrEvent   <= interrupt;
	IP2Bus_WrAck       <= BUS2IP_WrCE(0) or BUS2IP_WrCE(1);
	IP2Bus_RdAck       <= BUS2IP_RdCE(0) or BUS2IP_RdCE(1);
	IP2Bus_Error       <= '0';


end IMP;
