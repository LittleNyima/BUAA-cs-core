`timescale 1ns / 1ps

`include "define.v"

module CTRLD(
		input [31:0] iinstr,
		input iCMP_equal,
		output reg [2:0] oEXT_op,
		output reg [2:0] oNPC_sel
    );
	
	`INSTR_DEF
	
	wire [5:0] opcode, func;
	assign opcode = iinstr[31:26];
	assign func = iinstr[5:0];
	
	always@(*) begin
		`DECODE
		
		if(ori) oEXT_op = `EXT_UNSIGNED;
		else if(lw || sw) oEXT_op = `EXT_SIGNED;
		else if(lui) oEXT_op = `EXT_HIGHHALF;
		else oEXT_op = `EXT_UNSIGNED; // default
		
		if(addu || subu || ori || lw || sw || lui || nop) oNPC_sel = `NPC_SEQUENCE;
		else if(beq) oNPC_sel = (iCMP_equal)? `NPC_BRANCH : `NPC_SEQUENCE;
		else if(j || jal) oNPC_sel = `NPC_JUMP;
		else if(jr) oNPC_sel = `NPC_JUMPREG;
		else oNPC_sel = `NPC_SEQUENCE; // default
	end

endmodule

module CTRLE(
		input [31:0] iinstr,
		output reg oALU_src,
		output reg [2:0] oALU_op,
		output reg [1:0] oGRF_WD
	 );
	
	`INSTR_DEF
	
	wire [5:0] opcode, func;
	assign opcode = iinstr[31:26];
	assign func = iinstr[5:0];
	
	always@(*) begin
		`DECODE
		
		if(addu || lw || sw || lui || jr) oALU_op = `ALU_OP_ADD;
		else if(subu) oALU_op = `ALU_OP_SUB;
		else if(ori) oALU_op = `ALU_OP_OR;
		else oALU_op = `ALU_OP_ADD; // default
		
		if(addu || subu|| beq || jr) oALU_src = `ALU_SRC_GRF;
		else if(ori || sw || lw || lui) oALU_src = `ALU_SRC_EXT;
		else oALU_src = `ALU_SRC_GRF; // default
		
		if(addu || subu) oGRF_WD = `GRF_WD_RD;
		else if(ori || lw || lui) oGRF_WD = `GRF_WD_RT;
		else if(jal) oGRF_WD = `GRF_WD_1F;
		else oGRF_WD = `GRF_WD_RD; // default
	end
	
endmodule

module CTRLM(
		input [31:0] iinstr,
		output reg oDM_WE
	 );
	 
	`INSTR_DEF
	
	wire [5:0] opcode, func;
	assign opcode = iinstr[31:26];
	assign func = iinstr[5:0];
	
	always@(*) begin
		`DECODE
		
		oDM_WE = sw;
	end
	
endmodule

module CTRLW(
		input [31:0] iinstr,
		output reg [1:0] oGRF_WS,
		output reg oGRF_WE
	 );
	 
	`INSTR_DEF
	
	wire [5:0] opcode, func;
	assign opcode = iinstr[31:26];
	assign func = iinstr[5:0];
	
	always@(*) begin
		`DECODE
		
		if(addu || subu || ori || lui) oGRF_WS = `GRF_WS_ALU;
		else if(lw) oGRF_WS = `GRF_WS_DM;
		else if(jal) oGRF_WS = `GRF_WS_PC8;
		else oGRF_WS = `GRF_WS_ALU; // default
		
		oGRF_WE = addu || subu || ori || lw || lui || jal;
	end
	
endmodule
