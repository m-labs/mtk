-- Brief  : XIL_NPI VGA controller for displaying a frame buffer on VGA/TFT
-- Author : Matthias Alles
-- Date   : 2007-2008
--
-- The resolution is software controlled.
-- It is intended to be used with a MPMC3 or MPMC4 Memory core as an XIL_NPI component.
-- 32 Bit and 64 Bit NPI interfaces are supported.
-- The color format for one pixel is R=15 downto 11, G=10 downto 6, B=4 downto 0

--
-- Copyright (C) Genode Labs, Feske & Helmuth Systementwicklung GbR
--               http://www.genode-labs.com
--
-- This file is part of the Genode-FX package, which is distributed
-- under the terms of the GNU General Public License version 2.
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity npi_vga is
	generic( 
	C_PI_ADDR_WIDTH : integer := 32;  -- fixed for XIL_NPI components
	C_PI_DATA_WIDTH : integer := 32;  
	C_PI_BE_WIDTH   : integer := 4;   
	C_PI_RDWDADDR_WIDTH: integer := 4;-- fixed for XIL_NPI components
	C_NPI_PIXEL_CLK_RATIO : integer := 4
	); 
	Port (
	clk   : in std_logic;
	rst   : in std_logic; -- async reset

	frame_buffer_address : in std_logic_vector(31 downto 0);
	frame_buffer_address_valid : in std_logic;
	background_color : in std_logic_vector(15 downto 0);

	fast_clear_start_address : in std_logic_vector(31 downto 0);
	fast_clear_init          : in std_logic;
	fast_clear_pixels        : in std_logic_vector(18 downto 0);
	fast_clear_done          : out std_logic;

	enable_screen    : in std_logic;
	low_x_resolution : in std_logic;
	low_y_resolution : in std_logic;
	x_res : in unsigned(10 downto 0);
	x_max : in unsigned(10 downto 0);
	hsync_low  : in unsigned(10 downto 0);
	hsync_high : in unsigned(10 downto 0);
	y_res : in  unsigned(9 downto 0);
	y_max : in unsigned(9 downto 0);
	vsync_low  : in unsigned(9 downto 0);
	vsync_high : in unsigned(9 downto 0);

	frame_buffer_size : in unsigned(21 downto 0);
	bgnd_low  : in unsigned(9 downto 0);
	bgnd_high : in unsigned(9 downto 0);

	-- VGA/TFT signals
	tft_lcd_hsync : out std_logic;
	tft_lcd_vsync : out std_logic;
	tft_lcd_r     : out std_logic_vector(4 downto 0);
	tft_lcd_g     : out std_logic_vector(5 downto 0);
	tft_lcd_b     : out std_logic_vector(4 downto 0);
	tft_lcd_de    : out std_logic;
	
	-- MPMC Port Interface - Bus is prefixed with XIL_NPI_ 
	XIL_NPI_Addr : out std_logic_vector(C_PI_ADDR_WIDTH-1 downto 0); 
	XIL_NPI_AddrReq : out std_logic; 
	XIL_NPI_AddrAck : in std_logic; 
	XIL_NPI_RNW : out std_logic; 
	XIL_NPI_Size : out std_logic_vector(3 downto 0); 
	XIL_NPI_InitDone : in std_logic; 
	XIL_NPI_WrFIFO_Data : out std_logic_vector(C_PI_DATA_WIDTH-1 downto 0); 
	XIL_NPI_WrFIFO_BE : out std_logic_vector(C_PI_BE_WIDTH-1 downto 0); 
	XIL_NPI_WrFIFO_Push : out std_logic; 
	XIL_NPI_RdFIFO_Data : in std_logic_vector(C_PI_DATA_WIDTH-1 downto 0); 
	XIL_NPI_RdFIFO_Pop : out std_logic; 
	XIL_NPI_RdFIFO_RdWdAddr: in std_logic_vector(C_PI_RDWDADDR_WIDTH-1 downto 0); 
	XIL_NPI_WrFIFO_AlmostFull: in std_logic; 
	XIL_NPI_WrFIFO_Flush: out std_logic; 
--	XIL_NPI_RdFIFO_DataAvailable: in std_logic;   -- removed in mpmc3!!
	XIL_NPI_RdFIFO_Empty: in std_logic; 
	XIL_NPI_RdFIFO_Latency: in std_logic_vector(1 downto 0);
	XIL_NPI_RdFIFO_Flush: out std_logic 
);
end npi_vga;

architecture rtl of npi_vga is

	component vga_timing
	generic(
		C_NPI_PIXEL_CLK_RATIO : integer
	);
	port(
		clk   : in std_logic;
		rst   : in std_logic;
		run   : in std_logic;
		hsync : out std_logic;
		vsync : out std_logic;
		blank : out std_logic;
		frame : out std_logic;
		x_res : in unsigned(10 downto 0);
		x_max : in unsigned(10 downto 0);
		hsync_low  : in unsigned(10 downto 0);
		hsync_high : in unsigned(10 downto 0);
		y_res : in  unsigned(9 downto 0);
		y_max : in unsigned(9 downto 0);
		vsync_low  : in unsigned(9 downto 0);
		vsync_high : in unsigned(9 downto 0);
		x     : out std_logic_vector(10 downto 0);
		y     : out std_logic_vector(9 downto 0));
	end component;

	signal hsync : std_logic;
	signal vsync : std_logic;
	signal blank : std_logic;
	signal frame : std_logic;
	signal x_pos : std_logic_vector(10 downto 0);
	signal x_pos_t : std_logic_vector(10 downto 0);
	signal y_pos : std_logic_vector(9 downto 0);

	signal pixel_shift_register : std_logic_vector(C_PI_DATA_WIDTH-1 downto 0);
	signal run_timing : std_logic;

	signal pixels_requested : unsigned(8 downto 0);
	signal pixel_shift_register_content : unsigned(2 downto 0);

	signal wait_for_addr_ack : std_logic;

	signal XIL_NPI_RdFIFO_Pop_int : std_logic;
	signal XIL_NPI_RdFIFO_Pop_int_reg : std_logic;

	-- shadow reg is used to hold the content of the rdFIFO before the pixels are displayed
	-- the thing is that reading from the FIFO can have a latency of up to two clock cycles
	-- to cope with this we first store the pixel data in this register and copy it to pixel shift register
	signal shadow_reg : std_logic_vector(C_PI_DATA_WIDTH-1 downto 0);
	signal shadow_reg_filled : std_logic;

	type reading_fsm_type is (FIFO_REQ_0, FIFO_REQ_1, FIFO_REQ_2);
	signal reading_fsm : reading_fsm_type;

	signal blank_t : std_logic;

	-- how many pixels shall we keep in the FIFO?
	constant PIXEL_REQUEST_LOW : integer := 200;  -- FIFO MAX DEPTH = 512 (BRAM), or 32 (SRL16)
	-- how many pixels shall we request at once?
	constant REQUEST_SIZE : integer := 64; -- allowed values: 16 pixels, 32 pixels (since mpmc3), 64 pixels, 128 pixels

	-- used to detect timeouts from the MPMC2
	signal time_out_counter : unsigned(9 downto 0);

	signal screen_address_offset : unsigned(21 downto 0);
	signal fast_clear_offset : unsigned(21 downto 0);
	signal wait_for_addr_ack_wr : std_logic;
	signal fast_clear_pending : std_logic;
	signal fast_clear_pixels_cleared : unsigned(20 downto 0);

	-- we need to distinguish the x flag for a) fetching data and b) showing data
	-- the _dis flag can only be set during the vsync
	-- while the low_x_resolution_set flag has to be set as soon as we fetch data of the next screen.
	signal low_x_resolution_set : std_logic;
	signal low_x_resolution_set_dis : std_logic;
	signal low_y_resolution_set : std_logic;

	signal pixel_counter_x : unsigned(10 downto 0);
	signal line_displayed_once : std_logic;

	signal heal_the_world : std_logic;

begin

	vga_timing_inst : vga_timing 
	generic map(
		C_NPI_PIXEL_CLK_RATIO => C_NPI_PIXEL_CLK_RATIO
	)
	port map(
		clk => clk,
		rst => rst,
		run => run_timing,
		hsync => hsync,
		vsync => vsync,
		blank => blank,
		frame => frame,
		x_res => x_res,
		x_max => x_max,
		hsync_low  => hsync_low,
		hsync_high => hsync_high,
		y_res => y_res,
		y_max => y_max,
		vsync_low => vsync_low,
		vsync_high => vsync_high,
		x => x_pos,
		y => y_pos
	);

	-- This modules only reads
	XIL_NPI_WrFIFO_Flush <= '0';
	XIL_NPI_WrFIFO_BE    <= (others => '1');
	XIL_NPI_WrFIFO_Data  <= (others => '0');

	-- we request 8/32/64 words, each consisting of 32 bits (i.e. 16/64/128 pixels)
	XIL_NPI_Size <= std_logic_vector(to_unsigned(2, 4)) when REQUEST_SIZE = 16 else
	            std_logic_vector(to_unsigned(3, 4)) when REQUEST_SIZE = 32 else  -- only allowed since mpmc3
	            std_logic_vector(to_unsigned(4, 4)) when REQUEST_SIZE = 64 else
	            std_logic_vector(to_unsigned(5, 4)) when REQUEST_SIZE = 128;

	-- pop from the rdFIFO when the shadow_reg is empty and the rdFIFO is not empty
	XIL_NPI_RdFIFO_Pop <= XIL_NPI_RdFIFO_Pop_int;
	XIL_NPI_RdFIFO_Pop_int  <= '1' when reading_FSM = FIFO_REQ_0 and shadow_reg_filled = '0' and XIL_NPI_RdFIFO_Empty = '0' 
						else '0';

	tft_lcd_hsync <= hsync;
	tft_lcd_vsync <= vsync;

	process(clk, rst) is
		variable pixel_endian_inverse: std_logic_vector(15 downto 0);
	begin
		if rst = '1' then
			tft_lcd_r <= (others => '0');
			tft_lcd_g <= (others => '0');
			tft_lcd_b <= (others => '0');
			tft_lcd_de <= '0';
			pixels_requested <= (others => '0');
			run_timing <= '0';
			x_pos_t <= (others => '0');
			pixel_shift_register_content <= (others => '0');
			pixel_shift_register         <= (others => '0');
			screen_address_offset <= (others => '0');
			XIL_NPI_Addr <= (others => '0');
			XIL_NPI_AddrReq <= '0';
			wait_for_addr_ack <= '0';
			shadow_reg_filled <= '0';
			shadow_reg <= (others => '0');
			reading_fsm <= FIFO_REQ_0;
			blank_t <= '0';
			time_out_counter <= (others => '0');
			XIL_NPI_RdFIFO_Pop_int_reg <= '0';
			XIL_NPI_RdFIFO_Flush <= '0';
			XIL_NPI_RNW <= '0';
			XIL_NPI_WrFIFO_Push  <= '0';

			fast_clear_done <= '1';
			fast_clear_offset <= (others => '0');
			wait_for_addr_ack_wr <= '0';
			fast_clear_pending <= '0';
			fast_clear_pixels_cleared <= (others => '0');
			low_x_resolution_set <= '0';
			low_x_resolution_set_dis <= '0';
			low_y_resolution_set <= '0';

			pixel_counter_x <= (others => '0');
			line_displayed_once <= '0';
			heal_the_world <= '0';

		elsif clk'event and clk = '1' then

			heal_the_world <= '0';
			XIL_NPI_RdFIFO_Flush <= '0';

			-- start running the VGA timing when the first pixel data is already available
			if run_timing = '0' and shadow_reg_filled = '1' then
				run_timing <= '1';
				shadow_reg_filled <= '0';
				pixel_shift_register <= shadow_reg;
				pixel_shift_register_content <= to_unsigned(C_PI_DATA_WIDTH/16-1,3); -- two or four new pixels!
			end if;
			x_pos_t <= x_pos;
			if vsync = '0' then
				low_x_resolution_set_dis <= low_x_resolution_set;
			end if;

			-- in case there is space for new pixel data in the rdFIFO
			-- and we are currently not waiting for an request acknowledge 
			-- request new pixel data!
			if pixels_requested < PIXEL_REQUEST_LOW and 
			   wait_for_addr_ack = '0' and wait_for_addr_ack_wr = '0' then
				-- set the request and correct address
				XIL_NPI_AddrReq <= '1';
				XIL_NPI_RNW <= '1';
				-- remember that we are waiting for an acknowledge
				-- such that we do not enter this if branch again the next clk
				wait_for_addr_ack <= '1';
				-- and handle update the address of the frame buffer for the next time
				if screen_address_offset = frame_buffer_size then
					-- only update this bound after a whole picture was requested
					-- like this we stay synchronous even if the screen address is not set in the vsync
					XIL_NPI_Addr <= frame_buffer_address;
					screen_address_offset <= to_unsigned(REQUEST_SIZE*2, 22);
					pixel_counter_x <= to_unsigned(REQUEST_SIZE, 11);
					line_displayed_once <= '0';
					-- set the new resolution
					low_x_resolution_set <= low_x_resolution;
					low_y_resolution_set <= low_y_resolution;
				else
					XIL_NPI_Addr <= std_logic_vector(unsigned(frame_buffer_address) + screen_address_offset);
					screen_address_offset <= screen_address_offset + REQUEST_SIZE*2;
					-- in case we have to display each line twice
					-- test whether one line was displayed twice or not
					if low_y_resolution_set = '1' then
						-- test whether a whole line was displayed
						if (low_x_resolution_set = '0' and pixel_counter_x = x_res - REQUEST_SIZE) or
						   (low_x_resolution_set = '1' and pixel_counter_x = x_res/2 - REQUEST_SIZE) then
							pixel_counter_x <= (others => '0');
							if line_displayed_once = '0' then
								-- this was the first time the line was displayed
								-- so display it another time and reset the screen offset counter
								if low_x_resolution_set = '1' then
									screen_address_offset <= screen_address_offset - x_res + REQUEST_SIZE*2;
								else
									screen_address_offset <= screen_address_offset - x_res*2 + REQUEST_SIZE*2;
								end if;
								line_displayed_once <= '1';
							else
								line_displayed_once <= '0';
							end if;
						else
							pixel_counter_x <= pixel_counter_x + REQUEST_SIZE;
						end if;
					end if;
				end if;
			-----------------------------------------
			-- FAST CLEARING OF SCREEN FUNCTIONALITY
			-----------------------------------------
			elsif fast_clear_pending = '1' and wait_for_addr_ack = '0' and wait_for_addr_ack_wr = '0'
			        and fast_clear_pixels_cleared < unsigned(fast_clear_pixels) then
				XIL_NPI_AddrReq <= '1';
				XIL_NPI_Addr <= std_logic_vector(unsigned(fast_clear_start_address) + fast_clear_offset);
				XIL_NPI_RNW <= '0';
				fast_clear_offset <= fast_clear_offset + to_unsigned(REQUEST_SIZE*2,22);
				wait_for_addr_ack_wr <= '1';
			end if;

			if fast_clear_init = '1' then
				fast_clear_pending <= '1';
				fast_clear_done    <= '0';
			end if;

			if wait_for_addr_ack_wr = '1' and XIL_NPI_AddrAck = '1' then
				wait_for_addr_ack_wr <= '0';
				XIL_NPI_AddrReq <= '0';
				if fast_clear_pixels_cleared + REQUEST_SIZE >= unsigned(fast_clear_pixels) then
					fast_clear_pending <= '0';
					fast_clear_done <= '1'; -- initialize an interrupt
					fast_clear_pixels_cleared <= (others => '0');
					fast_clear_offset <= (others => '0');
				else
					fast_clear_pixels_cleared <= fast_clear_pixels_cleared + REQUEST_SIZE;
				end if;
			end if;
			-- fill the output FIFO as much as possible with zeros (black)
			if XIL_NPI_WrFIFO_AlmostFull = '0' and XIL_NPI_InitDone = '1' then
				XIL_NPI_WrFIFO_Push  <= '1';
			else
				XIL_NPI_WrFIFO_Push  <= '0';
			end if;

			-- are we reading an entry of the FIFO? 
			-- If so: four pixels are read (64 bit entries, 16 bit pixels)
			-- make this clocked, in mpmc3 empty-flag seems to be asynchronous
			-- we thus need to do this to achieve the target clock frequency
			XIL_NPI_RdFIFO_Pop_int_reg <= XIL_NPI_RdFIFO_Pop_int;
			if XIL_NPI_RdFIFO_Pop_int_reg = '1' then
				pixels_requested <= pixels_requested - C_PI_DATA_WIDTH/16;
			end if;
			-- was the read request acknowledged?
			if wait_for_addr_ack = '1' and XIL_NPI_AddrAck = '1' then
				-- yes: remove the request signal at once!
				XIL_NPI_AddrReq <= '0';
				wait_for_addr_ack <= '0';
				-- and there is new data in the FIFO soon!
				if XIL_NPI_RdFIFO_Pop_int_reg = '1' then
					-- when reading in the same clock cycle some pixels are removed at the same time
					pixels_requested <= pixels_requested + REQUEST_SIZE - C_PI_DATA_WIDTH/16;
				else
					pixels_requested <= pixels_requested + REQUEST_SIZE;
				end if;
			end if;

			-- check whether the FIFO is empty for a long time
			-- this happens when something is loaded with xmd
			-- for unknown reasons
			if XIL_NPI_RdFIFO_Empty = '1' and run_timing = '1' then
				time_out_counter <= time_out_counter + 1;
			else
				time_out_counter <= (others => '0');
			end if;


			-- READING FIFO OF MPMC2/3/4
			-- this is a small FSM that handles the latency of the rdFIFO
			-- according to the MPMC2-XIL_NPI manual three values for the latency are possible:
			-- 0: No latency at all
			-- 1: 1 clock cycle
			-- 2: 2 clock cycles
			case reading_FSM is

				when FIFO_REQ_0 =>
					-- if our shadow register is empty and data is available in the FIFO
					-- read data from the FIFO and store it in the shadow register
					if shadow_reg_filled = '0' and XIL_NPI_RdFIFO_Empty = '0' then
						-- here we also pop from the rdFIFO (async)
						-- in case of zero latency we are done here
						if XIL_NPI_RdFIFO_Latency = "00" then
							shadow_reg_filled <= '1';
							shadow_reg <= XIL_NPI_RdFIFO_Data;
						else
							-- else wait one or two clock cycles
							reading_FSM <= FIFO_REQ_1;
						end if;
					end if;

				when FIFO_REQ_1 =>
					-- one clock latency? Yes: we are done
					if XIL_NPI_RdFIFO_Latency = "01" then
						shadow_reg_filled <= '1';
						shadow_reg <= XIL_NPI_RdFIFO_Data;
						reading_FSM <= FIFO_REQ_0;
					else
						-- else wait another clock cycle
						reading_FSM <= FIFO_REQ_2;
					end if;

				when FIFO_REQ_2 =>
					-- two clock cycles latency is the maximum
					if XIL_NPI_RdFIFO_Latency = "10" then
						shadow_reg_filled <= '1';
						shadow_reg <= XIL_NPI_RdFIFO_Data;
						reading_FSM <= FIFO_REQ_0;
					end if;

			end case;

			-- TIMEOUT!!!!
			-- SO HEAL THE WORLD!
			if enable_screen = '0' or heal_the_world = '1' or time_out_counter = 1023 then
				time_out_counter <= (others => '0');
				wait_for_addr_ack <= '0';
				XIL_NPI_AddrReq <= '0';
				pixels_requested <= (others => '0');
				-- this resets the VGA timing
				run_timing <= '0';
				XIL_NPI_RdFIFO_Flush <= '1';
				screen_address_offset <= (others => '0');
				reading_fsm <= FIFO_REQ_0;
				x_pos_t <= (others => '0');
				pixel_counter_x <= (others => '0');
				line_displayed_once <= '0';
				shadow_reg_filled <= '0';
				reading_FSM <= FIFO_REQ_0;
			end if;
			blank_t <= blank;

			tft_lcd_de <= '0';
			-- display black only when a sync applies
			-- we take the delayed blank to guarantee a constant time for all pixels
			if blank_t = '1' or run_timing = '0' then
				tft_lcd_r <= (others => '0');
				tft_lcd_g <= (others => '0');
				tft_lcd_b <= (others => '0');
				-- or take the background color where possible
				-- switch of the background color when the electron ray runs to the start position
				if run_timing = '1' and hsync = '1' and (unsigned(y_pos) < bgnd_high or unsigned(y_pos) > bgnd_low) then
					tft_lcd_r <= background_color(15 downto 11);
					tft_lcd_g <= background_color(10 downto 5);
					tft_lcd_b <= background_color( 4 downto 0);
				end if;
			else
				-- we now display a new x, so display a new pixel
				-- the data enable flag has to be set in any case, since we want to display one pixel twice!
				if x_pos /= x_pos_t and run_timing = '1' then
					tft_lcd_de <= '1';
				end if;
				-- we now display a new x, so display a new pixel
				if (low_x_resolution_set_dis = '0' and x_pos /= x_pos_t and run_timing = '1') or
				   (low_x_resolution_set_dis = '1' and x_pos(10 downto 1) /= x_pos_t(10 downto 1) and run_timing = '1') then
					-- thus, shift the 64 bit wide pixel shift register by 16 bits
					-- and remember that one pixel less is pending in it
					pixel_shift_register(C_PI_DATA_WIDTH-16-1 downto 0) <= pixel_shift_register(C_PI_DATA_WIDTH-1 downto 16);
					pixel_shift_register_content <= pixel_shift_register_content - 1;
					-- read two/four new pixels from shadow register if we display the very last pixel of the pixel shift register
					if pixel_shift_register_content = 0 then
						if shadow_reg_filled = '1' then
							shadow_reg_filled <= '0';
							pixel_shift_register <= shadow_reg;
							pixel_shift_register_content <= to_unsigned(C_PI_DATA_WIDTH/16-1,3); -- two or four new pixels!
						else
							-- ERROR!! We need to use new pixels but they are not yet available!
							-- So initialize a reset of the screen display!
							heal_the_world <= '1';
						end if;
					end if;
				end if;
				-- inverse the bytes, due to little endianess
				pixel_endian_inverse := pixel_shift_register(7 downto 0) & pixel_shift_register(15 downto 8);
				tft_lcd_r <= pixel_endian_inverse(15 downto 11);
				tft_lcd_g <= pixel_endian_inverse(10 downto 5);
				tft_lcd_b <= pixel_endian_inverse(4 downto 0);
			end if;

		end if;
	end process;

end rtl;
