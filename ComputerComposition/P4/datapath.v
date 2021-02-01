`timescale 1ns / 1ps

module datapath(
		input in_IFU_SRC,
		input [1:0] in_IFU_nPC_sel,
		input in_GRF_WE,
		input [1:0] in_GRF_WD,
		input [1:0] in_GRF_WS,
		input [1:0] in_ALU_option,
		input in_ALU_src,
		input in_DM_WE,
		input [1:0] in_EXT_option,
		input clk,
		input reset,
		output [31:0] out_instr
    );

	wire [31:0] IFU_in;
	wire [31:0] IFU_out_instr;
	wire [31:0] IFU_out_PC4;
	wire [31:0] EXT_out;
	wire [31:0] GRF_out_rs;
	wire [31:0] GRF_out_rt;
	wire [4:0] GRF_in_rd_Addr;
	wire [31:0] GRF_in_Data;
	wire [31:0] ALU_in_opB;
	wire [31:0] ALU_out;
	wire ALU_out_zero;
	wire [31:0] DM_out;
	reg [4:0] CONST_0x1f = 5'h1f;
	
	assign out_instr = IFU_out_instr;
	
	MUX_2_32bits MMUX_IFU_IN(.in0(IFU_out_instr), .in1(GRF_out_rs), .in_sel(in_IFU_SRC), .out(IFU_in));
	
	IFU IIFU(
		.in_nPC_sel(in_IFU_nPC_sel), .in_zero(ALU_out_zero), .in32(IFU_in),
		.clk(clk), .reset(reset), .out_instr(IFU_out_instr), .out_PC4(IFU_out_PC4)
	);
	
	MUX_4_5bits MMUX_GRF_WD(
		.in0(IFU_out_instr[20:16]), .in1(IFU_out_instr[15:11]), .in2(CONST_0x1f), 
		.in_sel(in_GRF_WD), .out(GRF_in_rd_Addr)
	);
	
	MUX_4_32bits MMUX_GRF_DATA(
		.in0(ALU_out), .in1(DM_out), .in2(IFU_out_PC4),
		.in_sel(in_GRF_WS), .out(GRF_in_Data)
	);
	
	EXT_16TO32 EEXT(.in(IFU_out_instr[15:0]), .in_op(in_EXT_option), .out(EXT_out));
	
	GRF GGRF(
		.in_rs_Addr(IFU_out_instr[25:21]), .in_rt_Addr(IFU_out_instr[20:16]), 
		.in_rd_Addr(GRF_in_rd_Addr), .in_GRF_Data(GRF_in_Data), .in_PC4(IFU_out_PC4),
		.in_GRF_WE(in_GRF_WE), .clk(clk), .reset(reset),
		.out_rs_Data(GRF_out_rs), .out_rt_Data(GRF_out_rt)
	);
	
	MUX_2_32bits MMUX_ALU_src(.in0(GRF_out_rt), .in1(EXT_out), .in_sel(in_ALU_src), .out(ALU_in_opB));
	
	ALU AALU(
		.in_opA(GRF_out_rs), .in_opB(ALU_in_opB), .in_option(in_ALU_option), 
		.out(ALU_out), .out_zero(ALU_out_zero)
	);
	
	DM DDM(
		.in_Addr(ALU_out[11:2]), .in_Data(GRF_out_rt), .in_PC4(IFU_out_PC4),
		.in_DM_WE(in_DM_WE), .clk(clk), .reset(reset), .out(DM_out)
	);

endmodule
