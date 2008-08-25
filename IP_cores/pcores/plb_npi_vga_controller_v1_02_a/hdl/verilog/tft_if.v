//----------------------------------------------------------------------------
// TFT INTERFACE - Sub Level Module
//-----------------------------------------------------------------------------
//
//     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
//     SOLELY FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR
//     XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION
//     AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE, APPLICATION
//     OR STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS
//     IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
//     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
//     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
//     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
//     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
//     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
//     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//     FOR A PARTICULAR PURPOSE.
//     
//     (c) Copyright 2004 Xilinx, Inc.
//     All rights reserved.
// 
//----------------------------------------------------------------------------
// Filename:     tft_if.v
// 
// Description:    
//      This module takes as input, the TFT input signals and registers them.
//  Therefore all signals going to the TFT hardware is registered in the 
//      IOBs.
//
// Design Notes:
//      Wrapper type file to register all the inputs to the TFT hardware.
//   
//-----------------------------------------------------------------------------
// Structure:   
//              -- tft_if.v
//-----------------------------------------------------------------------------
// Author:              CJN
// History:
//      CJN, MM 3/02    -- First Release
//      CJN                             -- Second Release
//
//   
//
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Module Declaration
///////////////////////////////////////////////////////////////////////////////
`timescale 1 ns / 100 ps
module tft_if(
        clk,                    // I
        rst,                    // I
        HSYNC,                  // I
        VSYNC,                  // I
        DE,                     // I
        R0,                     // I
        R1,                     // I
        R2,                     // I
        R3,                     // I
        R4,                     // I
        R5,                     // I
        G0,                     // I
        G1,                     // I                                    
        G2,                     // I
        G3,                     // I
        G4,                     // I
        G5,                     // I
        B0,                     // I
        B1,                     // I
        B2,                     // I
        B3,                     // I
        B4,                     // I    
        B5,                     // I
        TFT_LCD_HSYNC,          // O
        TFT_LCD_VSYNC,          // O
        TFT_LCD_DE,             // O
        TFT_LCD_CLK_P,          // O
        TFT_LCD_CLK_N,          // O
        TFT_LCD_DATA0,          // O
        TFT_LCD_DATA1,          // O
        TFT_LCD_DATA2,          // O
        TFT_LCD_DATA3,          // O
        TFT_LCD_DATA4,          // O
        TFT_LCD_DATA5,          // O
        TFT_LCD_DATA6,          // O
        TFT_LCD_DATA7,          // O
        TFT_LCD_DATA8,          // O
        TFT_LCD_DATA9,          // O
        TFT_LCD_DATA10,         // O
        TFT_LCD_DATA11          // O
);

///////////////////////////////////////////////////////////////////////////////
// Port Declarations
///////////////////////////////////////////////////////////////////////////////
        input                   clk;
        input                   rst;
        input                   HSYNC;                          
        input                   VSYNC;                          
        input                   DE;     
        input                   R0;
        input                   R1;
        input                   R2;
        input                   R3;
        input                   R4;
        input                   R5;
        input                   G0;
        input                   G1;                             
        input                   G2;
        input                   G3;
        input                   G4;
        input                   G5;
        input                   B0;
        input                   B1;
        input                   B2;
        input                   B3;
        input                   B4;
        input                   B5;
        output                  TFT_LCD_HSYNC;
        output                  TFT_LCD_VSYNC;
        output                  TFT_LCD_DE;
        output                  TFT_LCD_CLK_P;
        output                  TFT_LCD_CLK_N;
        output                  TFT_LCD_DATA0;
        output                  TFT_LCD_DATA1;
        output                  TFT_LCD_DATA2;
        output                  TFT_LCD_DATA3;
        output                  TFT_LCD_DATA4;
        output                  TFT_LCD_DATA5;
        output                  TFT_LCD_DATA6;
        output                  TFT_LCD_DATA7;
        output                  TFT_LCD_DATA8;
        output                  TFT_LCD_DATA9;
        output                  TFT_LCD_DATA10;
        output                  TFT_LCD_DATA11;

///////////////////////////////////////////////////////////////////////////////
// Signal Declaration
///////////////////////////////////////////////////////////////////////////////
                
        ////////////////////////////////////////////////////////////////////////////
        // FDRE COMPONENT INSTANTIATION FOR IOB OUTPUT REGISTERS
        // -- All output to TFT/tft are registered
        ////////////////////////////////////////////////////////////////////////////
        FDRE FDRE_HSYNC (.Q(TFT_LCD_HSYNC), .C(~clk), .CE(1'b1), .R(rst), .D(HSYNC))     /* synthesis syn_useioff = 1 */; 
        FDRE FDRE_VSYNC (.Q(TFT_LCD_VSYNC), .C(~clk), .CE(1'b1), .R(rst), .D(VSYNC))     /* synthesis syn_useioff = 1 */;
        FDRE FDRE_DE    (.Q(TFT_LCD_DE),    .C(~clk), .CE(1'b1), .R(rst), .D(DE))    /* synthesis syn_useioff = 1 */;
 
        ODDR TFT_CLKP_ODDR (.Q(TFT_LCD_CLK_P), .C(clk), .CE(1'b1), .R(1'b0), .D1(1'b1), .D2(1'b0), .S(1'b0));
        ODDR TFT_CLKN_ODDR (.Q(TFT_LCD_CLK_N), .C(clk), .CE(1'b1), .R(1'b0), .D1(1'b0), .D2(1'b1), .S(1'b0));

        ODDR ODDR_TFT_DATA0   (.Q(TFT_LCD_DATA0),  .C(clk), .CE(1'b1), .R(~DE), .D2(1'b0), .D1(G2),     .S(1'b0));
        ODDR ODDR_TFT_DATA1   (.Q(TFT_LCD_DATA1),  .C(clk), .CE(1'b1), .R(~DE), .D2(1'b0), .D1(G3),     .S(1'b0));
        ODDR ODDR_TFT_DATA2   (.Q(TFT_LCD_DATA2),  .C(clk), .CE(1'b1), .R(~DE), .D2(B0),   .D1(G4),     .S(1'b0));
        ODDR ODDR_TFT_DATA3   (.Q(TFT_LCD_DATA3),  .C(clk), .CE(1'b1), .R(~DE), .D2(B1),   .D1(G5),     .S(1'b0));
        ODDR ODDR_TFT_DATA4   (.Q(TFT_LCD_DATA4),  .C(clk), .CE(1'b1), .R(~DE), .D2(B2),   .D1(1'b0),   .S(1'b0));
        ODDR ODDR_TFT_DATA5   (.Q(TFT_LCD_DATA5),  .C(clk), .CE(1'b1), .R(~DE), .D2(B3),   .D1(1'b0),   .S(1'b0));
        ODDR ODDR_TFT_DATA6   (.Q(TFT_LCD_DATA6),  .C(clk), .CE(1'b1), .R(~DE), .D2(B4),   .D1(R0),     .S(1'b0));
        ODDR ODDR_TFT_DATA7   (.Q(TFT_LCD_DATA7),  .C(clk), .CE(1'b1), .R(~DE), .D2(B5),   .D1(R1),     .S(1'b0));
        ODDR ODDR_TFT_DATA8   (.Q(TFT_LCD_DATA8),  .C(clk), .CE(1'b1), .R(~DE), .D2(1'b0), .D1(R2),     .S(1'b0));
        ODDR ODDR_TFT_DATA9   (.Q(TFT_LCD_DATA9),  .C(clk), .CE(1'b1), .R(~DE), .D2(1'b0), .D1(R3),     .S(1'b0));
        ODDR ODDR_TFT_DATA10  (.Q(TFT_LCD_DATA10), .C(clk), .CE(1'b1), .R(~DE), .D2(G0),   .D1(R4),     .S(1'b0));
        ODDR ODDR_TFT_DATA11  (.Q(TFT_LCD_DATA11), .C(clk), .CE(1'b1), .R(~DE), .D2(G1),   .D1(R5),     .S(1'b0));


endmodule
