`timescale 1ns / 1ps

module mips(
		input clk,
		input reset
    );

	wire IFU_SRC;
	wire [31:0] instr;
	wire [1:0]	IFU_nPC_sel;
	wire GRF_WE;
	wire [1:0] GRF_WD;
	wire [1:0] GRF_WS;
	wire [1:0] ALU_option;
	wire ALU_src;
	wire DM_WE;
	wire [1:0] EXT_option;
	
	controller CCONTROLLER(
		.in_opcode(instr[31:26]), .in_func(instr[5:0]), .out_IFU_src(IFU_SRC),
		.out_IFU_nPC_sel(IFU_nPC_sel), .out_GRF_WE(GRF_WE), .out_GRF_WD(GRF_WD), .out_GRF_WS(GRF_WS),
		.out_ALU_option(ALU_option), .out_ALU_src(ALU_src), .out_DM_WE(DM_WE), .out_EXT_option(EXT_option)
	);
	
	datapath DDATAPATH(
		.in_IFU_SRC(IFU_SRC), .in_IFU_nPC_sel(IFU_nPC_sel), .in_GRF_WE(GRF_WE), .in_GRF_WD(GRF_WD), 
		.in_GRF_WS(GRF_WS), .in_ALU_option(ALU_option), .in_ALU_src(ALU_src), .in_DM_WE(DM_WE), 
		.in_EXT_option(EXT_option), .clk(clk), .reset(reset), .out_instr(instr)
	);

endmodule
