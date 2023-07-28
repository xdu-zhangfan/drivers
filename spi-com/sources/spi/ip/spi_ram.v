/************************************************************\
 **     Copyright (c) 2012-2023 Anlogic Inc.
 **  All Right Reserved.\
\************************************************************/
/************************************************************\
 ** Log	:	This file is generated by Anlogic IP Generator.
 ** File	:	/data/Tasks/high_speed_acquisition_system/td/sources/spi/ip/spi_ram.v
 ** Date	:	2023 07 28
 ** TD version	:	5.6.71036
\************************************************************/

`timescale 1ns / 1ps

module spi_ram ( 
	doa, dia, addra, cea, clka, wea,
	dob, dib, addrb, ceb, clkb, web
);


	parameter DATA_WIDTH_A = 8; 
	parameter ADDR_WIDTH_A = 16;
	parameter DATA_DEPTH_A = 65536;
	parameter DATA_WIDTH_B = 32;
	parameter ADDR_WIDTH_B = 14;
	parameter DATA_DEPTH_B = 16384;
	parameter REGMODE_A    = "NOREG";
	parameter REGMODE_B    = "NOREG";
	parameter WRITEMODE_A  = "READBEFOREWRITE";
	parameter WRITEMODE_B  = "READBEFOREWRITE";
	parameter RESETMODE_A  = "ASYNC";
	parameter RESETMODE_B  = "ASYNC";

	output [DATA_WIDTH_A-1:0] doa;
	output [DATA_WIDTH_B-1:0] dob;


	input  [DATA_WIDTH_A-1:0] dia;
	input  [DATA_WIDTH_B-1:0] dib;
	input  [ADDR_WIDTH_A-1:0] addra;
	input  [ADDR_WIDTH_B-1:0] addrb;
	input  wea;
	input  web;
	input  cea;
	input  ceb;
	input  clka;
	input  clkb;




	PH1_LOGIC_ERAM #( .DATA_WIDTH_A(DATA_WIDTH_A),
				.DATA_WIDTH_B(DATA_WIDTH_B),
				.ADDR_WIDTH_A(ADDR_WIDTH_A),
				.ADDR_WIDTH_B(ADDR_WIDTH_B),
				.DATA_DEPTH_A(DATA_DEPTH_A),
				.DATA_DEPTH_B(DATA_DEPTH_B),
				.MODE("DP"),
				.REGMODE_A(REGMODE_A),
				.REGMODE_B(REGMODE_B),
				.WRITEMODE_A(WRITEMODE_A),
				.WRITEMODE_B(WRITEMODE_B),
				.IMPLEMENT("20K(FAST)"),
				.ECC_ENCODE("DISABLE"),
				.ECC_DECODE("DISABLE"),
				.CLKMODE("ASYNC"),
				.SSROVERCE("DISABLE"),
				.OREGSET_A("SET"),
				.OREGSET_B("SET"),
				.RESETMODE_A(RESETMODE_A),
				.RESETMODE_B(RESETMODE_B),
				.ASYNC_RESET_RELEASE_A("SYNC"),
				.ASYNC_RESET_RELEASE_B("SYNC"),
				.INIT_FILE("NONE"),
				.FILL_ALL("NONE"))
			inst(
				.dia(dia),
				.dib(dib),
				.addra(addra),
				.addrb(addrb),
				.cea(cea),
				.ceb(ceb),
				.ocea(1'b0),
				.oceb(1'b0),
				.clka(clka),
				.clkb(clkb),
				.wea(wea),
				.web(web),
				.bea(1'b0),
				.beb(1'b0),
				.rsta(1'b0),
				.rstb(1'b0),
				.doa(doa),
				.ecc_sbiterr(open),
				.ecc_dbiterr(open),
				.ecc_sbiterrinj('d0),
				.ecc_dbiterrinj('d0),
				.dob(dob));


endmodule