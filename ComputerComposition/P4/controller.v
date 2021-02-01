`timescale 1ns / 1ps

`ifndef ALU
`define ALU
`define ALU_OP_ADD 2'b00
`define ALU_OP_SUB 2'b01
`define ALU_OP_OR 2'b10
`define ALU_SRC_GRF 1'b0
`define ALU_SRC_EXT 1'b1
`endif

`ifndef EXT
`define EXT
`define EXT_UNSIGNED_EXTEND 2'b00
`define EXT_SIGNED_EXTEND 2'b01
`define EXT_HIGH_EXTEND 2'b10
`endif

`ifndef GRF
`define GRF
`define GRF_WD_RT 2'b00
`define GRF_WD_RD 2'b01
`define GRF_WD_RA 2'b10
`define GRF_WS_ALU 2'b00
`define GRF_WS_DM 2'b01
`define GRF_WS_PC4 2'b10
`endif

`ifndef IFU
`define IFU
`define IFU_SRC_INSTR 1'b0
`define IFU_SRC_GRF 1'b1
`define NPC_SEL_SEQUENCE 2'b00
`define NPC_SEL_BRANCHEQ 2'b01
`define NPC_SEL_JUMP 2'b10
`define NPC_SEL_JUMPREG 2'b11
`define TEXT_BASE 32'h3000
`endif

module controller(
		input [5:0] in_opcode,
		input [5:0] in_func,
		output reg out_IFU_src,
		output reg [1:0] out_IFU_nPC_sel,
		output reg out_GRF_WE,
		output reg [1:0] out_GRF_WD,
		output reg [1:0] out_GRF_WS,
		output reg [1:0] out_ALU_option,
		output reg out_ALU_src,
		output reg out_DM_WE,
		output reg [1:0] out_EXT_option
    );

	reg addu, subu, ori, sw, lw, beq, lui, jal, jr, nop;
	
	always@(*) begin
		addu = (in_opcode == 6'b000000 && in_func == 6'b100001);
		subu = (in_opcode == 6'b000000 && in_func == 6'b100011);
		ori = (in_opcode == 6'b001101);
		sw = (in_opcode == 6'b101011);
		lw = (in_opcode == 6'b100011);
		beq = (in_opcode == 6'b000100);
		lui = (in_opcode == 6'b001111);
		jal = (in_opcode == 6'b000011);
		jr = (in_opcode == 6'b000000 && in_func == 6'b001000);
		nop = (in_opcode == 6'b000000 && in_func == 6'b000000);
		
		if(beq || jal) out_IFU_src = `IFU_SRC_INSTR;
		else if(jr) out_IFU_src = `IFU_SRC_GRF;
		
		if(addu || subu || ori || sw || lw || lui || nop) out_IFU_nPC_sel = `NPC_SEL_SEQUENCE;
		else if(beq) out_IFU_nPC_sel = `NPC_SEL_BRANCHEQ;
		else if(jal) out_IFU_nPC_sel = `NPC_SEL_JUMP;
		else if(jr) out_IFU_nPC_sel = `NPC_SEL_JUMPREG;
		
		out_GRF_WE = addu || subu || ori || lw || lui || jal;
		
		if(ori || lw || lui) out_GRF_WD = `GRF_WD_RT;
		else if(addu || subu) out_GRF_WD = `GRF_WD_RD;
		else if(jal) out_GRF_WD = `GRF_WD_RA;
		
		if(addu || subu || ori || lui) out_GRF_WS = `GRF_WS_ALU;
		else if(lw) out_GRF_WS = `GRF_WS_DM;
		else if(jal) out_GRF_WS = `GRF_WS_PC4;
		
		if(addu || sw || lw || lui || jr) out_ALU_option = `ALU_OP_ADD;
		else if(subu || beq) out_ALU_option = `ALU_OP_SUB;
		else if(ori) out_ALU_option = `ALU_OP_OR;
		
		if(addu || subu|| beq || jr) out_ALU_src = `ALU_SRC_GRF;
		else if(ori || sw || lw || lui) out_ALU_src = `ALU_SRC_EXT;
		
		out_DM_WE = sw;
		
		if(ori) out_EXT_option = `EXT_UNSIGNED_EXTEND;
		else if(sw || lw) out_EXT_option = `EXT_SIGNED_EXTEND;
		else if(lui) out_EXT_option = `EXT_HIGH_EXTEND;
	end

endmodule
