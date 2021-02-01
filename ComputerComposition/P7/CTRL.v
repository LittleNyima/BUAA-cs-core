`timescale 1ns / 1ps

`include "define.v"
module CTRLD(
		input [31:0] iinstr,
		input [7:0] iCMPout,
		output reg [2:0] oEXTop,
		output reg [2:0] oNPC_sel,
		output reg isBJ,
		output reg RI
    );

	`INSTRUCTIONS
	`SPLIT
	
	reg recognize, instr1, instr2, instr3, instr4;
	
	initial begin
		instr1 = 1'b0;
		instr2 = 1'b0;
		instr3 = 1'b0;
		instr4 = 1'b0;
		recognize = 1'b0;
		oEXTop = 3'b0;
		oNPC_sel = 3'b0;
		isBJ = 1'b0;
		RI = 1'b0;
	end
	
	always@(*) begin
		`DECODE
		
		if(andi || ori || xori) oEXTop = `EXT_UNSIGNED;
		else if(lb || lbu || lh || lhu || lw || sb || sh || sw || addi || addiu || slti || sltiu) oEXTop = `EXT_SIGNED;
		else if(sll || srl || sra) oEXTop = `EXT_SHIFTAMT;// no use
		else if(lui) oEXTop = `EXT_HIGHHALF;
		else oEXTop = `EXT_UNSIGNED;
		
		if(lb || lbu || lh || lhu || lw || sb || sh || sw) oNPC_sel = `NPC_SEQUENCE;
		if(add || addu || sub || subu || sll || srl || sra || sllv || srlv || srav) oNPC_sel = `NPC_SEQUENCE;
		if(and_ || or_ || xor_ || nor_ || mult || multu || div || divu) oNPC_sel = `NPC_SEQUENCE;
		if(addi || addiu || andi || ori || xori || lui) oNPC_sel = `NPC_SEQUENCE;
		if(slt || slti || sltiu || sltu || mfhi || mflo || mthi || mtlo || nop) oNPC_sel = `NPC_SEQUENCE;
		if(mtc0 || mfc0) oNPC_sel = `NPC_SEQUENCE;
		if(jalr || jr) oNPC_sel = `NPC_JUMPREG;
		if(j || jal) oNPC_sel = `NPC_JUMP;
		if(beq) oNPC_sel = (iCMPout[`CMP_EQUAL])? `NPC_BRANCH : `NPC_SEQUENCE;
		if(bne) oNPC_sel = (iCMPout[`CMP_NEQUAL])? `NPC_BRANCH : `NPC_SEQUENCE;
		if(blez) oNPC_sel = (iCMPout[`CMP_RSLEZ])? `NPC_BRANCH : `NPC_SEQUENCE;
		if(bgtz) oNPC_sel = (iCMPout[`CMP_RSGTZ])? `NPC_BRANCH : `NPC_SEQUENCE;
		if(bltz) oNPC_sel = (iCMPout[`CMP_RSLTZ])? `NPC_BRANCH : `NPC_SEQUENCE;
		if(bgez) oNPC_sel = (iCMPout[`CMP_RSGEZ])? `NPC_BRANCH : `NPC_SEQUENCE;
		if(eret) oNPC_sel = `NPC_ERET;
		
		isBJ = beq || bne || blez || bgtz || bltz ||	bgez || j || jal || jalr || jr;
		
		instr1 = lb || lbu || lh || lhu || lw || sb || sh || sw || add || addu || sub || subu || sll || srl || sra || sllv || srlv || srav;
		instr2 = and_ || or_ || xor_ || nor_ || mult || multu || div || divu || addi || addiu || andi || ori || xori || lui;
		instr3 = slt || slti || sltiu || sltu || beq || bne || blez || bgtz || bltz || bgez || j || jal || jalr || jr;
		instr4 = mfhi || mflo || mthi || mtlo || nop || eret || mfc0 || mtc0;
		
		recognize = instr1 || instr2 || instr3 || instr4;
		RI = ~recognize;
	end

endmodule

module CTRLE(
		input [31:0] iinstr,
		output reg [4:0] oALUop,
		output reg [1:0] oGRF_WD,
		output reg oALUsrc,
		output reg oMD_sel,
		output reg LOADtype,
		output reg SAVEtype
    );
	 
	`INSTRUCTIONS
	`SPLIT
	
	initial begin
		oALUop = 4'b0;
		oGRF_WD = 3'b0;
		oALUsrc = 1'b0;
		oMD_sel = 1'b0;
		LOADtype = 1'b0;
		SAVEtype = 1'b0;
	end
	
	always@(*) begin
		`DECODE
		
		if(lb || lbu || lh || lhu || lw || sb || sh || sw) oALUop = `ALU_OP_ADD;
		else if(add || addi) oALUop = `ALU_OP_ADD;
		else if(addu || addiu || lui || mtc0 || mfc0) oALUop = `ALU_OP_ADDU;
		else if(sub) oALUop = `ALU_OP_SUB;
		else if(subu) oALUop = `ALU_OP_SUBU;
		else if(sll) oALUop = `ALU_OP_SLL;
		else if(srl) oALUop = `ALU_OP_SRL;
		else if(sra) oALUop = `ALU_OP_SRA;
		else if(sllv) oALUop = `ALU_OP_SLLV;
		else if(srlv) oALUop = `ALU_OP_SRLV;
		else if(srav) oALUop = `ALU_OP_SRAV;
		else if(and_ || andi) oALUop = `ALU_OP_AND;
		else if(or_ || ori) oALUop = `ALU_OP_OR;
		else if(xor_ || xori) oALUop = `ALU_OP_XOR;
		else if(nor_) oALUop = `ALU_OP_NOR;
		else if(slt || slti) oALUop = `ALU_OP_SLT;
		else if(sltu || sltiu) oALUop = `ALU_OP_SLTU;
		else oALUop = `ALU_OP_ADDU;
		
		if(add || addu || sub || subu || sll | srl || sra || sllv || srlv || srav) oGRF_WD = `GRF_WD_RD;
		else if(and_ || or_ || xor_ || nor_ || mfhi || mflo || jalr || slt || sltu) oGRF_WD = `GRF_WD_RD;
		else if(lb || lbu || lh || lhu || lw || addi || addiu || andi || ori || xori || lui || mfc0) oGRF_WD = `GRF_WD_RT;
		else if(slti || sltiu) oGRF_WD = `GRF_WD_RT;
		else if(jal) oGRF_WD = `GRF_WD_1F;
		
		if(add || addu || sub || subu || sllv || srlv || srav || sll | srl || sra) oALUsrc = `ALU_SRC_GRF;
		else if(and_ || or_ || xor_ || nor_ || mult || multu || div || divu || mthi || mtlo) oALUsrc = `ALU_SRC_GRF;
		else if(lb || lbu || lh || lhu || lw || addi || addiu || andi || ori || xori || lui) oALUsrc = `ALU_SRC_EXT;
		else if(slti || sltiu || sb || sh || sw) oALUsrc = `ALU_SRC_EXT;
		else oALUsrc = `ALU_SRC_GRF;
		
		if(mfhi) oMD_sel = `MD_SEL_HI;
		else if(mflo) oMD_sel = `MD_SEL_LO;
		
		LOADtype = lb || lbu || lh || lhu || lw;
		SAVEtype = sb || sh || sw;
	end

endmodule

module CTRLM(
		input [31:0] iinstr,
		output reg [3:0] octrl,
		output reg CP0_WE
    );

	`INSTRUCTIONS
	`SPLIT
	
	initial begin 
		octrl = 4'b0;
		CP0_WE = 1'b0;
	end
	
	always@(*) begin
		`DECODE
		
		CP0_WE = mtc0;
		
		if(sb) octrl = `DM_SB;
		else if(sh) octrl = `DM_SH;
		else if(sw) octrl = `DM_SW;
		else if(lb) octrl = `DM_LB;
		else if(lbu) octrl = `DM_LBU;
		else if(lh) octrl = `DM_LH;
		else if(lhu) octrl = `DM_LHU;
		else if(lw) octrl = `DM_LW;
		else if(mfc0) octrl = `CP0_MFC0;
		else octrl = `DM_NONE;
	end

endmodule

module CTRLW(
		input [31:0] iinstr,
		input [7:0] iCMPout,
		output reg [1:0] oGRF_WS,
		output reg oGRF_WE
    );

	`INSTRUCTIONS
	`SPLIT
	
	reg GRF_WE1, GRF_WE2, GRF_WE3;
	
	always@(*) begin
		`DECODE
		
		
		if(add || addu || sub || subu || sll || srl || sra || sllv || srlv || srav) oGRF_WS = `GRF_WS_ALU;
		else if(and_ || or_ || xor_ || nor_ || addi || addiu || andi || ori || xori || lui) oGRF_WS = `GRF_WS_ALU;
		else if(slt || slti || sltiu || sltu) oGRF_WS = `GRF_WS_ALU;
		else if(lb || lbu || lh || lhu || lw || mfc0) oGRF_WS = `GRF_WS_DM;
		else if(jal || jalr) oGRF_WS = `GRF_WS_PC8;
		else if(mfhi || mflo) oGRF_WS = `GRF_WS_MD;
		
		GRF_WE1 = lb || lbu || lh || lhu || lw || jal || jalr || mfhi || mflo || slt || slti || sltiu || sltu;
		GRF_WE2 = add || addu || sub || subu || sll || srl || sra || sllv || srlv || srav;
		GRF_WE3 = and_ || or_ || xor_ || nor_ || addi || addiu || andi || ori || xori || lui || mfc0;
		oGRF_WE = GRF_WE1 || GRF_WE2 || GRF_WE3;
	end

endmodule
