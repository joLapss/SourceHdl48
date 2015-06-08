/********************************************************************************************************

-- École de technologie supérieur
--		Kevin Parent Legault
--		Jonathan Lapointe
-- Create Date:  2015-06-02
-- Module Name: display.c
-- Project Name: lab2n
-- Target Devices: Altera cyclone V SOPC
-- Tool versions: NIOS II v14.1
-- Description: fichier de branchement de l'architecture vers l'extérieur
	

-- Revision: v1
-- Revision 0.01 - File Created
-- Additional Comments:


-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;



entity top is 
 port (
			clock_50 	: in std_logic;
			--Boutons
			KEY 			: in std_logic_vector(3 downto 0);
			--écran VGA couleurs
			vga_r 		: out std_logic_vector(7 downto 0);
			vga_b 		: out std_logic_vector(7 downto 0);
			vga_g 		: out std_logic_vector(7 downto 0);
			--écran VGA horloge
			vga_clk 	: out std_logic;
			--écran VGA synchro
			vga_blank_n : out std_logic;
			vga_hs 		: out std_logic;
			vga_vs 		: out std_logic;
			vga_sync_n 	: out std_logic;
			--broches souris
			ps2_clk 	: inout std_logic;
			--ps2_clk2 	: inout std_logic;
			ps2_dat 	: inout std_logic;
			--ps2_dat2 	: inout std_logic;
			--Broches allants vers SDRAM
			dram_clk 	: out std_logic;
			dram_addr 	: out std_logic_vector(12 downto 0);
			dram_dq 		: inout std_logic_vector(15 downto 0);
			dram_ba 		: out std_logic_vector(1 downto 0);
			dram_cas_n 	: out std_logic;
			dram_cke 	: out std_logic;
			dram_cs_n 	: out std_logic;
			dram_we_n 	: out std_logic;
			dram_udqm 	: out std_logic;
			dram_ldqm 	: out std_logic;
			dram_ras_n 	: out std_logic
			
			
			
			
			
			
			
 
 );
 end top;
 
 architecture structural of top is
	component lab2q is
        port (
            clk_clk       : in    std_logic:= 'X';             -- clk
            vga_out_CLK   : out   std_logic;                                        -- CLK
            vga_out_HS    : out   std_logic;                                        -- HS
            vga_out_VS    : out   std_logic;                                        -- VS
            vga_out_BLANK : out   std_logic;                                        -- BLANK
            vga_out_SYNC  : out   std_logic;                                        -- SYNC
			--écran VGA couleurs
            vga_out_R     : out   std_logic_vector(7 downto 0);                     -- R
            vga_out_G     : out   std_logic_vector(7 downto 0);                     -- G
            vga_out_B     : out   std_logic_vector(7 downto 0);                     -- B
			--Broches allants vers SDRAM
            sram_addr     : out   std_logic_vector(12 downto 0);                    -- addr
            sram_ba       : out   std_logic_vector(1 downto 0);                     -- ba
            sram_cas_n    : out   std_logic;                                        -- cas_n
            sram_cke      : out   std_logic;                                        -- cke
            sram_cs_n     : out   std_logic;                                        -- cs_n
            sram_dq       : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
            sram_dqm      : out   std_logic_vector(1 downto 0);                     -- dqm
            sram_ras_n    : out   std_logic;                                        -- ras_n
            sram_we_n     : out   std_logic;                                        -- we_n
            reset_reset_n : in    std_logic := 'X';             					-- reset_n
			--broches souris
            mouse_CLK     : inout std_logic := 'X';            						-- CLK		
            mouse_DAT     : inout std_logic := 'X';             					-- DAT
			--Broches allants vers SDRAM
		    clk_sdram_clk : out   std_logic 
        );
    end component lab2q;
	signal sdram_dqm : std_logic_vector(1 downto 0);
	begin 
	
    nios_system : component lab2q
        port map (
            clk_clk       => clock_50 ,       				--     clk.clk
			--écran VGA horloge
            vga_out_CLK   => vga_clk,   						-- vga_out.CLK
            vga_out_HS    => vga_hs,    						--        .HS
            vga_out_VS    => vga_vs,    						--        .VS
            vga_out_BLANK => vga_blank_n, 					    --        .BLANK
            vga_out_SYNC  => vga_sync_n,  					    --        .SYNC
			--écran VGA couleurs
            vga_out_R     => vga_r,     						--        .R
            vga_out_G     => vga_g,     						--        .G
            vga_out_B     => vga_b,     						--        .B
			--Broches allants vers SDRAM
            sram_addr     => dram_addr,     					--    sram.addr
            sram_ba       => dram_ba,       					--        .ba
            sram_cas_n    => dram_cas_n,    					--        .cas_n
            sram_cke      => dram_cke,      					--        .cke
            sram_cs_n     => dram_cs_n,    		 			    --        .cs_n
            sram_dq       => dram_dq,       					--        .dq
            sram_dqm   	  => sdram_dqm, 						--        .dqm
            sram_ras_n    => dram_ras_n,    					--        .ras_n
            sram_we_n     => dram_we_n,     					--        .we_n
            reset_reset_n => KEY(0), 							--   reset.reset_n
			--broches souris
            mouse_CLK     => ps2_clk ,     					    --   mouse.CLK
            mouse_DAT     => ps2_dat,       					--        .DAT
			clk_sdram_clk => dram_clk  						    -- clk_sdram.cl
        );

		--Broches allants vers SDRAM
		dram_udqm <= sdram_dqm(1);
		dram_ldqm <= sdram_dqm(0);
	


			
		end structural;
	 
	 
	 
	 
