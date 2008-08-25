-- Component : VGA timing generation
-- Author    : Matthias Alles
-- Date      : 2007-2008
--
-- This component generates a generic screen timing for VGA/TFT monitors.
-- The input signals give the screen characteristics.
-- C_NPI_PIXEL_CLK_RATIO gives the ratio between clk and the pixel clock.
-- The component performs "ticks" in the timing only every
-- C_NPI_PIXEL_CLK_RATIOth clock cycle.

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

entity vga_timing is
	generic ( 
		C_NPI_PIXEL_CLK_RATIO : in integer
	);
	Port (
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
end vga_timing;

architecture rtl of vga_timing is

	signal x_counter: unsigned(10 downto 0);
	signal y_counter: unsigned(9 downto 0);

	signal pixel_clk_counter : unsigned(2 downto 0);

begin

	-- connect to output ports
	x <= std_logic_vector(x_counter);
	y <= std_logic_vector(y_counter);


	generate_timing: process(clk, rst) is
		variable release_blank_force : std_logic;
	begin
		if rst = '1' then
			x_counter <= (others => '0');
			y_counter <= (others => '0');
			hsync     <= '1';
			vsync     <= '1';
			blank     <= '0';
			frame     <= '0';
			pixel_clk_counter <= "110";

		elsif clk'event and clk = '1' then

			-- counter to downscale the used clock to the pixel clock
			if run = '1' then
				if pixel_clk_counter = C_NPI_PIXEL_CLK_RATIO - 1 then
					pixel_clk_counter <= (others => '0');
				else
					pixel_clk_counter <= pixel_clk_counter + 1;
				end if;
			end if;

			-- in case we are not running set these values always to the reset values
			if run = '0' then
				x_counter <= (others => '0');
				y_counter <= (others => '0');
				hsync     <= '1';
				vsync     <= '1';
				blank     <= '0';
				frame     <= '0';
				pixel_clk_counter <= "110";
			end if;

			release_blank_force := '0';

			-- only do this, every ith clock cycle (i.e. when the pixel clk is active).
			if pixel_clk_counter = 0 and run = '1' then

				-- generate timing signals
				if x_counter >= x_res - 1 then
					blank <= '1';
					if x_counter >= hsync_low and x_counter <= hsync_high  then
						hsync <= '0';
					else
						hsync <= '1';
					end if;
				else
					blank <= '0';
					hsync <= '1';
				end if;

				-- handle counters
				if x_counter = x_max then
					x_counter <= (others => '0');
					if y_counter = y_res - 1 then
						blank <= '1';
					else
						blank <= '0';
						release_blank_force := '1'; -- This set of blank to '0' is a must
					end if;
					if y_counter = y_max then
						y_counter <= (others => '0');
					else
						y_counter <= y_counter + 1;
						release_blank_force := '0';
					end if;
				else
					x_counter <= x_counter + 1;
				end if;

				if y_counter >= y_res then   -- vertical sync in action
					if release_blank_force = '0' then
						blank <= '1';
					end if;
					if y_counter >= vsync_low and y_counter <= vsync_high then
						vsync <= '0';
					else
						vsync <= '1';
					end if;

				end if;

			end if;
		end if;
	end process generate_timing;

end rtl;
