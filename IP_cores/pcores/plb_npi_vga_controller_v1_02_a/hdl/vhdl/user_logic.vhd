-- Brief  : NPI VGA controller user logic
-- Author : Matthias Alles
-- Date   : 2007-2008
--
-- This component contains the control registers for the NPI_VGA controller.
-- It also takes care about using VGA or DVI output.

--
-- Copyright (C) Genode Labs, Feske & Helmuth Systementwicklung GbR
--               http://www.genode-labs.com
--
-- This file is part of the Genode-FX package, which is distributed
-- under the terms of the GNU General Public License version 2.
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library proc_common_v2_00_a;
use proc_common_v2_00_a.proc_common_pkg.all;


entity user_logic is
	generic
	(
	-- ADD USER GENERICS BELOW THIS LINE ---------------
	--USER generics added here
	C_PI_ADDR_WIDTH : integer := 32;  -- fixed for XIL_NPI components
	C_PI_DATA_WIDTH : integer := 64;  -- fixed for XIL_NPI components (since mpmc3 also 32 allowed)
	C_PI_BE_WIDTH   : integer := 8;   -- fixed for XIL_NPI components (since mpmc3 also 4  allowed)
	C_PI_RDWDADDR_WIDTH: integer := 4;-- fixed for XIL_NPI components
	C_NPI_PIXEL_CLK_RATIO : integer := 4;
	C_USE_VGA_OUT : boolean := true;
	C_PIXEL_CLK_GREATER_65MHZ : boolean := false;
	-- ADD USER GENERICS ABOVE THIS LINE ---------------

	-- DO NOT EDIT BELOW THIS LINE ---------------------
	-- Bus protocol parameters, do not add to or delete
	 C_SLV_DWIDTH                   : integer              := 32;
	 C_NUM_REG                      : integer              := 11;
	 C_NUM_INTR                     : integer              := 1
	-- DO NOT EDIT ABOVE THIS LINE ---------------------
 );
 port
 (
	-- ADD USER PORTS BELOW THIS LINE ------------------
	--USER ports added here
	NPI_clk : in std_logic;
	NPI_rst : in std_logic;
	-- VGA/TFT signals
 	clk_pixel     : in  std_logic;
	tft_lcd_hsync : out std_logic;
	tft_lcd_vsync : out std_logic;
	tft_lcd_r : out std_logic_vector(5 downto 2);
	tft_lcd_g : out std_logic_vector(5 downto 2);
	tft_lcd_b : out std_logic_vector(5 downto 2);
	tft_lcd_data  : out std_logic_vector(11 downto 0);
	tft_lcd_de    : out std_logic;
	tft_lcd_clk_n : out std_logic;
	tft_lcd_clk_p : out std_logic;
	tft_lcd_reset : out std_logic;
	tft_lcd_sda   : out std_logic;
	tft_lcd_scl   : out std_logic;
	
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
	XIL_NPI_RdFIFO_Empty: in std_logic; 
	XIL_NPI_RdFIFO_Latency: in std_logic_vector(1 downto 0);
	XIL_NPI_RdFIFO_Flush: out std_logic;
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

	--USER signal declarations added here, as needed for user logic

	------------------------------------------
	-- Signals for user logic slave model s/w accessible register example
	------------------------------------------
	signal slv_reg0                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg1                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg2                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg3                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg4                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg5                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg6                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg7                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg8                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg9                       : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg10                      : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_reg_write_sel              : std_logic_vector(0 to 10);
	signal slv_reg_read_sel               : std_logic_vector(0 to 10);
	signal slv_ip2bus_data                : std_logic_vector(0 to C_SLV_DWIDTH-1);
	signal slv_read_ack                   : std_logic;
	signal slv_write_ack                  : std_logic;

	------------------------------------------
	-- Signals for user logic interrupt example
	------------------------------------------
	signal intr_flags                   : std_logic_vector(0 to C_NUM_INTR-1);

	signal frame_buffer_address       : std_logic_vector(31 downto 0);
	signal frame_buffer_address_valid : std_logic;
	signal background_color  : std_logic_vector(15 downto 0);

	signal fast_clear_start_address : std_logic_vector(31 downto 0);
	signal fast_clear_init          : std_logic;
	signal fast_clear_pixels        : std_logic_vector(18 downto 0); --  640*480 maximum
	signal fast_clear_done          : std_logic;

	signal low_x_resolution : std_logic;
	signal low_y_resolution : std_logic;
	signal enable_screen    : std_logic;

	component npi_vga is
		generic( 
		C_PI_ADDR_WIDTH : integer := 32;  -- fixed for XIL_NPI components
		C_PI_DATA_WIDTH : integer := 64;  -- fixed for XIL_NPI components
		C_PI_BE_WIDTH   : integer := 8;   -- fixed for XIL_NPI components
		C_PI_RDWDADDR_WIDTH: integer := 4;-- fixed for XIL_NPI components
		C_FB_BASE_ADDR : std_logic_vector := X"00000000";
		C_NPI_PIXEL_CLK_RATIO : integer := 133
		); 
		Port (
		clk   : in std_logic;
		rst   : in std_logic; -- async reset

		frame_buffer_address : in std_logic_vector(31 downto 0);
		frame_buffer_address_valid : in std_logic;
		background_color : in std_logic_vector(15 downto 0);
	
		fast_clear_start_address : in std_logic_vector(31 downto 0);
		fast_clear_init          : in std_logic;
		fast_clear_pixels        : in std_logic_vector(18 downto 0); --  640*480 maximum
		fast_clear_done : out std_logic;

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
		XIL_NPI_RdFIFO_Empty: in std_logic; 
		XIL_NPI_RdFIFO_Latency: in std_logic_vector(1 downto 0);
		XIL_NPI_RdFIFO_Flush: out std_logic 
	);
	end component;

	component iic_init port(
		clk     : in  std_logic;
		reset_n : in  std_logic;
		Pixel_clk_greater_than_65MHz: in  std_logic;
		SDA     : out std_logic;
		SCL     : out std_logic;
		done    : out std_logic);
	end component;

	component tft_if port(
		clk:                    in std_logic;
		rst:                    in std_logic;
		HSYNC:                  in std_logic;
		VSYNC:                  in std_logic;
		DE:                     in std_logic;
		R0:                     in std_logic;
		R1:                     in std_logic;
		R2:                     in std_logic;
		R3:                     in std_logic;
		R4:                     in std_logic;
		R5:                     in std_logic;
		G0:                     in std_logic;
		G1:                     in std_logic;                                   
		G2:                     in std_logic;
		G3:                     in std_logic;
		G4:                     in std_logic;
		G5:                     in std_logic;
		B0:                     in std_logic;
		B1:                     in std_logic;
		B2:                     in std_logic;
		B3:                     in std_logic;
		B4:                     in std_logic;   
		B5:                     in std_logic;
		TFT_LCD_HSYNC:          out std_logic;
		TFT_LCD_VSYNC:          out std_logic;
		TFT_LCD_DE:             out std_logic;
		TFT_LCD_CLK_P:          out std_logic;
		TFT_LCD_CLK_N:          out std_logic;
		TFT_LCD_DATA0:          out std_logic;
		TFT_LCD_DATA1:          out std_logic;
		TFT_LCD_DATA2:          out std_logic;
		TFT_LCD_DATA3:          out std_logic;
		TFT_LCD_DATA4:          out std_logic;
		TFT_LCD_DATA5:          out std_logic;
		TFT_LCD_DATA6:          out std_logic;
		TFT_LCD_DATA7:          out std_logic;
		TFT_LCD_DATA8:          out std_logic;
		TFT_LCD_DATA9:          out std_logic;
		TFT_LCD_DATA10:         out std_logic;
		TFT_LCD_DATA11:         out std_logic);
	end component;

	signal dvi_init_done : std_logic;
	signal pixel_clk_greater_than_65mhz : std_logic;

	signal tft_lcd_data0 : std_logic;
	signal tft_lcd_data1 : std_logic;
	signal tft_lcd_data2 : std_logic;
	signal tft_lcd_data3 : std_logic;
	signal tft_lcd_data4 : std_logic;
	signal tft_lcd_data5 : std_logic;
	signal tft_lcd_data6 : std_logic;
	signal tft_lcd_data7 : std_logic;
	signal tft_lcd_data8 : std_logic;
	signal tft_lcd_data9 : std_logic;
	signal tft_lcd_data10 : std_logic;
	signal tft_lcd_data11 : std_logic;

	signal vsync_rst : std_logic;
	signal fast_clear_done_t : std_logic;

	signal tft_lcd_hsync_int   : std_logic;
	signal tft_lcd_vsync_int   : std_logic;
	signal tft_lcd_vsync_int_t : std_logic;

	signal tft_lcd_r_int : std_logic_vector(4 downto 0);
	signal tft_lcd_g_int : std_logic_vector(5 downto 0);
	signal tft_lcd_b_int : std_logic_vector(4 downto 0);

	signal tft_lcd_r_t1 : std_logic_vector(4 downto 0);
	signal tft_lcd_g_t1 : std_logic_vector(5 downto 0);
	signal tft_lcd_b_t1 : std_logic_vector(4 downto 0);
	signal tft_lcd_r_t2 : std_logic_vector(4 downto 0);
	signal tft_lcd_g_t2 : std_logic_vector(5 downto 0);
	signal tft_lcd_b_t2 : std_logic_vector(4 downto 0);
	signal tft_lcd_hsync_t1   : std_logic;
	signal tft_lcd_vsync_t1   : std_logic;
	signal tft_lcd_hsync_t2   : std_logic;
	signal tft_lcd_vsync_t2   : std_logic;

	signal de_npi_clk   : std_logic;
	signal de_pixel_clk : std_logic;
	signal de_sync_pipe : std_logic_vector(2 downto 0);
	signal de_level_flag: std_logic;
	signal rst_n : std_logic;

	signal x_res : unsigned(10 downto 0);
	signal x_max : unsigned(10 downto 0);
	signal hsync_low  : unsigned(10 downto 0);
	signal hsync_high : unsigned(10 downto 0);
	signal y_res : unsigned(9 downto 0);
	signal y_max : unsigned(9 downto 0);
	signal vsync_low  : unsigned(9 downto 0);
	signal vsync_high : unsigned(9 downto 0);
	signal frame_buffer_size : unsigned(21 downto 0);
	signal bgnd_low  : unsigned(9 downto 0);
	signal bgnd_high : unsigned(9 downto 0);

begin

	pixel_clk_greater_than_65mhz <= '1' when C_PIXEL_CLK_GREATER_65MHZ = true else '0';

	dvi_out: if C_USE_VGA_OUT = false generate
		dvi_iic_init: iic_init port map(
			clk     => Bus2IP_Clk,
			reset_n => rst_n,
			Pixel_clk_greater_than_65MHz => pixel_clk_greater_than_65mhz,
			SDA     =>  TFT_LCD_SDA,
			SCL     =>  TFT_LCD_SCL,
			done    =>  dvi_init_done);
	
		rst_n <= not BUS2IP_Reset;
		tft_lcd_reset <= rst_n;

		-- synchronize the high frequency signals to the pixel clock
		process(NPI_clk, Bus2IP_Reset) is
		begin
			if Bus2IP_Reset = '1' then
				de_level_flag <= '0';
			elsif NPI_clk'event and NPI_clk = '1' then
				if de_npi_clk = '1' then
					de_level_flag <= not de_level_flag;
				end if;
			end if;
		end process;

		process(clk_pixel, Bus2IP_Reset) is
		begin
			if Bus2IP_Reset = '1' then
				de_sync_pipe <= (others => '0');
				tft_lcd_r_t1 <= (others => '0');
				tft_lcd_g_t1 <= (others => '0');
				tft_lcd_b_t1 <= (others => '0');
				tft_lcd_r_t2 <= (others => '0');
				tft_lcd_g_t2 <= (others => '0');
				tft_lcd_b_t2 <= (others => '0');
				tft_lcd_hsync_t1 <= '0';
				tft_lcd_hsync_t2 <= '0';
				tft_lcd_vsync_t1 <= '0';
				tft_lcd_vsync_t2 <= '0';
			elsif clk_pixel'event and clk_pixel = '1' then
				de_sync_pipe(2 downto 0) <= de_sync_pipe(1 downto 0) & de_level_flag;
				tft_lcd_r_t1 <= tft_lcd_r_int;
				tft_lcd_g_t1 <= tft_lcd_g_int;
				tft_lcd_b_t1 <= tft_lcd_b_int;
				tft_lcd_r_t2 <= tft_lcd_r_t1;
				tft_lcd_g_t2 <= tft_lcd_g_t1;
				tft_lcd_b_t2 <= tft_lcd_b_t1;
				tft_lcd_hsync_t1 <= tft_lcd_hsync_int;
				tft_lcd_vsync_t1 <= tft_lcd_vsync_int;
				tft_lcd_hsync_t2 <= tft_lcd_hsync_t1;
				tft_lcd_vsync_t2 <= tft_lcd_vsync_t1;
			end if;
		end process;

		de_pixel_clk <= '1' when de_sync_pipe(2) /= de_sync_pipe(1) else '0';
	
		tft_if_inst:  tft_if port map(
			clk   => clk_pixel,
			rst   => Bus2IP_Reset,
			HSYNC => tft_lcd_hsync_t2,
			VSYNC => tft_lcd_vsync_t2,
			DE    => de_pixel_clk,
			R0    => '0',
			R1    => tft_lcd_r_t2(0),
			R2    => tft_lcd_r_t2(1),
			R3    => tft_lcd_r_t2(2),
			R4    => tft_lcd_r_t2(3),
			R5    => tft_lcd_r_t2(4),
			G0    => tft_lcd_g_t2(0),
			G1    => tft_lcd_g_t2(1),
			G2    => tft_lcd_g_t2(2),
			G3    => tft_lcd_g_t2(3),
			G4    => tft_lcd_g_t2(4),
			G5    => tft_lcd_g_t2(5),
			B0    => '0',
			B1    => tft_lcd_b_t2(0),
			B2    => tft_lcd_b_t2(1),
			B3    => tft_lcd_b_t2(2),
			B4    => tft_lcd_b_t2(3),
			B5    => tft_lcd_b_t2(4),
			TFT_LCD_HSYNC  => tft_lcd_hsync,
			TFT_LCD_VSYNC  => tft_lcd_vsync,
			TFT_LCD_DE     => tft_lcd_de,
			TFT_LCD_CLK_P  => tft_lcd_clk_p,
			TFT_LCD_CLK_N  => tft_lcd_clk_n,
			TFT_LCD_DATA0  => tft_lcd_data0,
			TFT_LCD_DATA1  => tft_lcd_data1,
			TFT_LCD_DATA2  => tft_lcd_data2,
			TFT_LCD_DATA3  => tft_lcd_data3,
			TFT_LCD_DATA4  => tft_lcd_data4,
			TFT_LCD_DATA5  => tft_lcd_data5,
			TFT_LCD_DATA6  => tft_lcd_data6,
			TFT_LCD_DATA7  => tft_lcd_data7,
			TFT_LCD_DATA8  => tft_lcd_data8,
			TFT_LCD_DATA9  => tft_lcd_data9,
			TFT_LCD_DATA10 => tft_lcd_data10,
			TFT_LCD_DATA11 => tft_lcd_data11);
	
		tft_lcd_data(0) <= tft_lcd_data0;
		tft_lcd_data(1) <= tft_lcd_data1;
		tft_lcd_data(2) <= tft_lcd_data2;
		tft_lcd_data(3) <= tft_lcd_data3;
		tft_lcd_data(4) <= tft_lcd_data4;
		tft_lcd_data(5) <= tft_lcd_data5;
		tft_lcd_data(6) <= tft_lcd_data6;
		tft_lcd_data(7) <= tft_lcd_data7;
		tft_lcd_data(8) <= tft_lcd_data8;
		tft_lcd_data(9) <= tft_lcd_data9;
		tft_lcd_data(10) <= tft_lcd_data10;
		tft_lcd_data(11) <= tft_lcd_data11;
	end generate;

	-- when we use the VGA output don't modify the sync signals
	vga_out: if C_USE_VGA_OUT = true generate
		tft_lcd_vsync <= tft_lcd_vsync_int;
		tft_lcd_hsync <= tft_lcd_hsync_int;
		tft_lcd_r     <= tft_lcd_r_int(4 downto 1);
		tft_lcd_g     <= tft_lcd_g_int(5 downto 2);
		tft_lcd_b     <= tft_lcd_b_int(4 downto 1);
	end generate;


	--USER logic implementation added here
	npi_vga_controller_inst : npi_vga
	generic map
	(
	 C_PI_ADDR_WIDTH => C_PI_ADDR_WIDTH,
	 C_PI_DATA_WIDTH => C_PI_DATA_WIDTH,
	 C_PI_BE_WIDTH   => C_PI_BE_WIDTH,
	 C_PI_RDWDADDR_WIDTH => C_PI_RDWDADDR_WIDTH,
	 C_NPI_PIXEL_CLK_RATIO => C_NPI_PIXEL_CLK_RATIO
	)
	port map
	(
	 clk => NPI_clk,
	 rst => NPI_rst,

	 frame_buffer_address => frame_buffer_address,
	 frame_buffer_address_valid => frame_buffer_address_valid,
	 background_color => background_color,

	 fast_clear_start_address => fast_clear_start_address,
	 fast_clear_init => fast_clear_init,
	 fast_clear_pixels => fast_clear_pixels,
	 fast_clear_done => fast_clear_done,

	 low_x_resolution => low_x_resolution,
	 low_y_resolution => low_y_resolution,
	 enable_screen => enable_screen,
	 x_res => x_res,
	 x_max => x_max,
	 hsync_low  => hsync_low,
	 hsync_high => hsync_high,
	 y_res => y_res,
	 y_max => y_max,
	 vsync_low => vsync_low,
	 vsync_high => vsync_high,
	 frame_buffer_size => frame_buffer_size,
	 bgnd_low => bgnd_low,
	 bgnd_high => bgnd_high,


	-- VGA/TFT signals
	 tft_lcd_hsync => tft_lcd_hsync_int,
	 tft_lcd_vsync => tft_lcd_vsync_int,
	 tft_lcd_r => tft_lcd_r_int,
	 tft_lcd_g => tft_lcd_g_int,
	 tft_lcd_b => tft_lcd_b_int,
	 tft_lcd_de => de_npi_clk,

	-- MPMC Port Interface - Bus is prefixed with XIL_NPI_ 
	 XIL_NPI_Addr => XIL_NPI_Addr,
	 XIL_NPI_AddrReq  => XIL_NPI_AddrReq,
	 XIL_NPI_AddrAck => XIL_NPI_AddrAck,
	 XIL_NPI_RNW => XIL_NPI_RNW,
	 XIL_NPI_Size => XIL_NPI_Size,
	 XIL_NPI_InitDone => XIL_NPI_InitDone,
	 XIL_NPI_WrFIFO_Data => XIL_NPI_WrFIFO_Data,
	 XIL_NPI_WrFIFO_BE => XIL_NPI_WrFIFO_BE,
	 XIL_NPI_WrFIFO_Push => XIL_NPI_WrFIFO_Push,
	 XIL_NPI_RdFIFO_Data => XIL_NPI_RdFIFO_Data,
	 XIL_NPI_RdFIFO_Pop => XIL_NPI_RdFIFO_Pop,
	 XIL_NPI_RdFIFO_RdWdAddr => XIL_NPI_RdFIFO_RdWdAddr,
	 XIL_NPI_WrFIFO_AlmostFull => XIL_NPI_WrFIFO_AlmostFull,
	 XIL_NPI_WrFIFO_Flush => XIL_NPI_WrFIFO_Flush,
	 XIL_NPI_RdFIFO_Empty => XIL_NPI_RdFIFO_Empty,
	 XIL_NPI_RdFIFO_Latency => XIL_NPI_RdFIFO_Latency,
	 XIL_NPI_RdFIFO_Flush  => XIL_NPI_RdFIFO_Flush
	);

	-- REGISTER MAP FOR THIS CORE:
	-- BASEADDR + 
	-- 0x00 : Framebuffer address (R/W), 32bit
	-- 0x04 : Background Color (R/W), lowest 16 bit only
	-- 0x08 : Start Address for fast memory clear
	-- 0x0C : Number of pixels to clear in fast mode. Writing launches the clearing, 19 Bit
	-- 0x10 : Control Register:
	--        Bit 0 : low x resolution (320 instead of 640)
	--        Bit 1 : low y resolution (240 instead of 480)
	--        Bit 2 : run timing
	--        Bit 3 : enable vblank interrupt (TO BE DONE...)
	-- 0x14 : (low)  Screen width in pixel  ( 10 downto  0) 
	--        (high) Screen heigth in pixel ( 25 downto 16)
 	--        ATTENTION, in case of low resolutions (bit 0 or bit 1 in 0x10 set)
	--                   here the base type is required (i.e. twice the resolution)
	-- 0x18 : Frame buffer size (bytes)     ( 21 downto  0)
	-- 0x1C : (low)  hsync low              ( 10 downto  0)
	--      : (high) hsync high             ( 26 downto 16)
	-- 0x20 : (low)  vsync low              (  9 downto  0)
	--      : (high) vsync high             ( 25 downto 16)
	-- 0x24 : (low)  x max                  ( 10 downto  0)
	--      : (high) y max                  ( 25 downto 16)
	-- 0x28 : (low)  bgnd low               (  9 downto  0)
	--      : (high) bgnd high              ( 25 downto 16)

	-- reverse the std_logic_type
	process(slv_reg0, slv_reg1, slv_reg2, slv_reg3, slv_reg5, slv_reg6, slv_reg7, slv_reg8, slv_reg9, slv_reg10) is
		variable vector10 : std_logic_vector( 9 downto 0);
		variable vector11 : std_logic_vector(10 downto 0);
		variable vector22 : std_logic_vector(21 downto 0);
	begin
		for i in 0 to 10 loop
			vector11(i) := slv_reg5(31-i);
		end loop;
		x_res <= unsigned(vector11);
		for i in 0 to 10 loop
			vector11(i) := slv_reg7(31-i);
		end loop;
		hsync_low <= unsigned(vector11);
		for i in 0 to 10 loop
			vector11(i) := slv_reg7(15-i);
		end loop;
		hsync_high <= unsigned(vector11);
		for i in 0 to 10 loop
			vector11(i) := slv_reg9(31-i);
		end loop;
		x_max <= unsigned(vector11);
		for i in 0 to 9 loop
			vector10(i) := slv_reg5(15-i);
		end loop;
		y_res <= unsigned(vector10);
		for i in 0 to 9 loop
			vector10(i) := slv_reg8(31-i);
		end loop;
		vsync_low <= unsigned(vector10);
		for i in 0 to 9 loop
			vector10(i) := slv_reg8(15-i);
		end loop;
		vsync_high <= unsigned(vector10);
		for i in 0 to 9 loop
			vector10(i) := slv_reg9(15-i);
		end loop;
		y_max <= unsigned(vector10);
		for i in 0 to 9 loop
			vector10(i) := slv_reg10(31-i);
		end loop;
		bgnd_low <= unsigned(vector10);
		for i in 0 to 9 loop
			vector10(i) := slv_reg10(15-i);
		end loop;
		bgnd_high <= unsigned(vector10);
		for i in 0 to 21 loop
			vector22(i) := slv_reg6(31-i);
		end loop;
		frame_buffer_size <= unsigned(vector22);
		for i in 0 to 31 loop
			frame_buffer_address(i) <= slv_reg0(31 - i);
		end loop;
		for i in 0 to 15 loop
			background_color(i) <= slv_reg1(31 - i);
		end loop;
		for i in 0 to 31 loop
			fast_clear_start_address(i) <= slv_reg2(31 - i);
		end loop;
		for i in 0 to 18 loop
			fast_clear_pixels(i) <= slv_reg3(31 - i);
		end loop;
	end process;

	-- set flags according to register contents
	low_x_resolution <= slv_reg4(31);
	low_y_resolution <= slv_reg4(30);
	enable_screen    <= slv_reg4(29);

	slv_reg_write_sel <= Bus2IP_WrCE(0 to 10);
	slv_reg_read_sel  <= Bus2IP_RdCE(0 to 10);
	slv_write_ack     <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1) or Bus2IP_WrCE(2) or Bus2IP_WrCE(3) or Bus2IP_WrCE(4) or
	                     Bus2IP_WrCE(5) or Bus2IP_WrCE(6) or Bus2IP_WrCE(7) or Bus2IP_WrCE(8) or Bus2IP_WrCE(9) or
	                     Bus2IP_WrCE(10);
	slv_read_ack      <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1) or Bus2IP_RdCE(2) or Bus2IP_RdCE(3) or Bus2IP_RdCE(4) or
	                     Bus2IP_RdCE(5) or Bus2IP_RdCE(6) or Bus2IP_RdCE(7) or Bus2IP_RdCE(8) or Bus2IP_RdCE(9) or
	                     Bus2IP_RdCE(10);


	-- implement slave model software accessible register(s)
	SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is
	begin
		if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
			if Bus2IP_Reset = '1' then
				slv_reg0 <= (others => '0');
				slv_reg1 <= (others => '0');
				slv_reg2 <= (others => '0');
				slv_reg3 <= (others => '0');
				slv_reg4 <= (others => '0');
				slv_reg5 <= (others => '0');
				slv_reg6 <= (others => '0');
				slv_reg7 <= (others => '0');
				slv_reg8 <= (others => '0');
				slv_reg9 <= (others => '0');
				slv_reg10 <= (others => '0');
				frame_buffer_address_valid <= '0';
				fast_clear_init <= '0';
			else
				frame_buffer_address_valid <= '0';
				fast_clear_init <= '0';
				case slv_reg_write_sel is
					when "10000000000" =>
						slv_reg0 <= Bus2IP_Data;
						frame_buffer_address_valid <= '1';
					when "01000000000" =>
						slv_reg1 <= Bus2IP_Data;
					when "00100000000" =>
						slv_reg2 <= Bus2IP_Data;
					when "00010000000" =>
						slv_reg3 <= Bus2IP_Data;
						fast_clear_init <= '1';
					when "00001000000" =>
						slv_reg4 <= Bus2IP_Data;
					when "00000100000" =>
						slv_reg5 <= Bus2IP_Data;
					when "00000010000" =>
						slv_reg6 <= Bus2IP_Data;
					when "00000001000" =>
						slv_reg7 <= Bus2IP_Data;
					when "00000000100" =>
						slv_reg8 <= Bus2IP_Data;
					when "00000000010" =>
						slv_reg9 <= Bus2IP_Data;
					when "00000000001" =>
						slv_reg10 <= Bus2IP_Data;
					when others => null;
				end case;
			end if;
		end if;
	end process SLAVE_REG_WRITE_PROC;

	-- implement slave model software accessible register(s) read mux
	SLAVE_REG_READ_PROC : process( slv_reg_read_sel, slv_reg0, slv_reg1, slv_reg2, slv_reg3, slv_reg4, slv_reg5, slv_reg6, slv_reg7, slv_reg8, slv_reg9, slv_reg10 ) is
	begin
		case slv_reg_read_sel is
			when "10000000000" => slv_ip2bus_data <= slv_reg0;
			when "01000000000" => slv_ip2bus_data <= slv_reg1;
			when "00100000000" => slv_ip2bus_data <= slv_reg2;
			when "00010000000" => slv_ip2bus_data <= slv_reg3;
			when "00001000000" => slv_ip2bus_data <= slv_reg4;
			when "00000100000" => slv_ip2bus_data <= slv_reg5;
			when "00000010000" => slv_ip2bus_data <= slv_reg6;
			when "00000001000" => slv_ip2bus_data <= slv_reg7;
			when "00000000100" => slv_ip2bus_data <= slv_reg8;
			when "00000000010" => slv_ip2bus_data <= slv_reg9;
			when "00000000001" => slv_ip2bus_data <= slv_reg10;
			when others => slv_ip2bus_data <= (others => '0');
		end case;
	end process SLAVE_REG_READ_PROC;

	-- Interrupt generation process
	INTR_PROC : process( Bus2IP_Clk ) is
	begin
		if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
			if ( Bus2IP_Reset = '1' ) then
				intr_flags <= (others => '0');
				tft_lcd_vsync_int_t <= '0';
				fast_clear_done_t <= '1';
			else
				-- ATTENTION: tft_lcd_vsync_int and fast_clear_done
				-- are generated using another CLOCK!!
				tft_lcd_vsync_int_t <= tft_lcd_vsync_int;
				-- interrupt when vsync detected
				if tft_lcd_vsync_int_t = '1' and tft_lcd_vsync_int = '0' then
					intr_flags(0) <= '1';
				else
					intr_flags(0) <= '0';
				end if;

				fast_clear_done_t <= fast_clear_done;
				-- interrupt when fast clear finished
				if fast_clear_done_t = '0' and fast_clear_done = '1' then
					intr_flags(1) <= '1';
				else
					intr_flags(1) <= '0';
				end if;
			end if;
		end if;

	end process INTR_PROC;

	IP2Bus_IntrEvent <= intr_flags;

	------------------------------------------
	-- Example code to drive IP to Bus signals
	------------------------------------------
	IP2Bus_Data  <= slv_ip2bus_data when slv_read_ack = '1' else
				 (others => '0');

	IP2Bus_WrAck <= slv_write_ack;
	IP2Bus_RdAck <= slv_read_ack;
	IP2Bus_Error <= '0';

end IMP;
