`timescale 1ns / 1ps

module mips(
		input clk,
		input reset,
		input interrupt,
		output [31:0] addr
    );
	
	wire [31:0] PR2BR_Data, BR2PR_Data, BR2DEV_Data;
	wire [31:0] DEV02BR_Data, DEV12BR_Data;
	wire [31:0] PR2BR_Addr;
	wire [29:0] BR2DEV_Addr;
	wire [5:0] INTRQ;
	wire [3:0] BE;
	wire [2:0] DEV_INTRQ;
	wire DEV_WE0, DEV_WE1, DEV_INTRQ0, DEV_INTRQ1;
	
	assign DEV_INTRQ[0] = DEV_INTRQ0;
	assign DEV_INTRQ[1] = DEV_INTRQ1;
	assign DEV_INTRQ[2] = interrupt;
	 
	CPU _CPU(
		.clk(clk), .reset(reset), .iDEV_Data(BR2PR_Data), .iINTRQ(INTRQ), 
		.oDEV_Data(PR2BR_Data), .oDEV_Addr(PR2BR_Addr), .oDEV_BE(BE), .runtime_PC(addr)
	);
	
	BRIDGE _BRIDGE(
		.iPR_Addr(PR2BR_Addr), .iPR_Data(PR2BR_Data), .iDEV_Data0(DEV02BR_Data), .iDEV_Data1(DEV12BR_Data), 
		.iPR_BE(BE), .iINTRQ(DEV_INTRQ), .oPR_Data(BR2PR_Data), .oDEV_Data(BR2DEV_Data), .oDEV_Addr(BR2DEV_Addr), 
		.oDEV_WE0(DEV_WE0), .oDEV_WE1(DEV_WE1), .oINTRQ(INTRQ)
	);
	 
	TIMER _TIMER0(
		.clk(clk), .reset(reset), .Addr(BR2DEV_Addr), .WE(DEV_WE0), 
		.Din(BR2DEV_Data), .Dout(DEV02BR_Data), .IRQ(DEV_INTRQ0)
	);
	 
	TIMER _TIMER1(
		.clk(clk), .reset(reset), .Addr(BR2DEV_Addr), .WE(DEV_WE1), 
		.Din(BR2DEV_Data), .Dout(DEV12BR_Data), .IRQ(DEV_INTRQ1)
	);
	
endmodule
