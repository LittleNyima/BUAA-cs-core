`timescale 1ns / 1ps

`include "define.v"

module HAZARD(
		input [31:0] iinstr,
		input clk,
		input reset,
		// stall control
		input iMDbusy,
		output oPC_en,
		output oREGD_en,
		output oREGE_clear,
		// bypass control
		output [2:0] oMFGRFo1,
		output [2:0] oMFGRFo2,
		output [2:0] oMFALUi1,
		output [2:0] oMFALUi2,
		output oMFDMin
    );

	wire [4:0] wA1D, wA2D, wA3D;
	wire wTURS0D, wTURS1D, wTURT0D, wTURT1D, wTURT2D, wMDD;
	wire [2:0] wTND;

	ATDECODER _ATDECODER(
		.iinstr(iinstr), .oA1(wA1D), .oA2(wA2D), .oA3(wA3D), .oMDD(wMDD), .Tuse_rs0(wTURS0D), 
		.Tuse_rs1(wTURS1D), .Tuse_rt0(wTURT0D), .Tuse_rt1(wTURT1D), .Tuse_rt2(wTURT2D), .Tnew(wTND)
	);
	
	wire [4:0] wA1E, wA2E, wA3E;
	wire wTURS0E, wTURS1E, wTURT0E, wTURT1E, wTURT2E;
	wire [2:0] wTNE;
	wire wREGE_clear, wATREGE_clear;
	assign oREGE_clear = wREGE_clear;
	assign wATREGE_clear = wREGE_clear || reset;
	
	ATREG _ATREGE(
		.iA1(wA1D), .iA2(wA2D), .iA3(wA3D), .iTuse_rs0(wTURS0D), .iTuse_rs1(wTURS1D), 
		.iTuse_rt0(wTURT0D), .iTuse_rt1(wTURT1D), .iTuse_rt2(wTURT2D), .iTnew(wTND), .clk(clk), 
		.reset(wATREGE_clear), .oA1(wA1E), .oA2(wA2E), .oA3(wA3E), .oTuse_rs0(wTURS0E), 
		.oTuse_rs1(wTURS1E), .oTuse_rt0(wTURT0E), .oTuse_rt1(wTURT1E), .oTuse_rt2(wTURT2E),
		.oTnew(wTNE)
	);
	
	wire [4:0] wA1M, wA2M, wA3M;
	wire wTURS0M, wTURS1M, wTURT0M, wTURT1M, wTURT2M;
	wire [2:0] wTNM;
	
	ATREG _ATREGM(
		.iA1(wA1E), .iA2(wA2E), .iA3(wA3E), .iTuse_rs0(wTURS0E), .iTuse_rs1(wTURS1E), 
		.iTuse_rt0(wTURT0E), .iTuse_rt1(wTURT1E), .iTuse_rt2(wTURT2E), .iTnew(wTNE), 
		.clk(clk), .reset(reset), .oA1(wA1M), .oA2(wA2M), .oA3(wA3M), .oTuse_rs0(wTURS0M), 
		.oTuse_rs1(wTURS1M), .oTuse_rt0(wTURT0M), .oTuse_rt1(wTURT1M), .oTuse_rt2(wTURT2M),
		.oTnew(wTNM)
	);
					  
	wire [4:0] wA1W, wA2W, wA3W;
	wire wTURS0W, wTURS1W, wTURT0W, wTURT1W, wTURT2W;
	wire [2:0] wTNW;
	
	ATREG _ATREGW(
		.iA1(wA1M), .iA2(wA2M), .iA3(wA3M), .iTuse_rs0(wTURS0M), .iTuse_rs1(wTURS1M), 
		.iTuse_rt0(wTURT0M), .iTuse_rt1(wTURT1M), .iTuse_rt2(wTURT2M), .iTnew(wTNM), 
		.clk(clk), .reset(reset), .oA1(wA1W), .oA2(wA2W), .oA3(wA3W), .oTuse_rs0(wTURS0W), 
		.oTuse_rs1(wTURS1W), .oTuse_rt0(wTURT0W), .oTuse_rt1(wTURT1W), .oTuse_rt2(wTURT2W),
		.oTnew(wTNW)
	);
					  
	STALL _STALL(
		.iA1(wA1D), .iA2(wA2D), .iA3E(wA3E), .iTnewE(wTNE), .iA3M(wA3M), .iTnewM(wTNM), 
		.iMDD(wMDD), .iMDbusy(iMDbusy), .iTuse_rs0(wTURS0D), .iTuse_rs1(wTURS1D), 
		.iTuse_rt0(wTURT0D), .iTuse_rt1(wTURT1D), .iTuse_rt2(wTURT2D), .oPC_en(oPC_en), 
		.oREGD_en(oREGD_en), .oREGE_clear(wREGE_clear)
	);
	
	BYPASSCTRL _BYPASSCTRL(
		.iA3M(wA3M), .iTnewM(wTNM), .iA3W(wA3W), .iTnewW(wTNW), .iA1D(wA1D), .iA1E(wA1E), 
		.iA2D(wA2D), .iA2E(wA2E), .iA2M(wA2M), .oMFGRFo1(oMFGRFo1), .oMFGRFo2(oMFGRFo2), 
		.oMFALUi1(oMFALUi1), .oMFALUi2(oMFALUi2), .oMFDMin(oMFDMin)
	);
					  
endmodule

module ATDECODER(
		input [31:0] iinstr,
		output reg [4:0] oA1,
		output reg [4:0] oA2,
		output reg [4:0] oA3,
		output reg oMDD,
		output reg Tuse_rs0,
		output reg Tuse_rs1,
		output reg Tuse_rt0,
		output reg Tuse_rt1,
		output reg Tuse_rt2,
		output reg [2:0] Tnew
	 );
	
	`INSTRUCTIONS
	`SPLIT
	
	reg Tuse_rs1_p1, Tuse_rs1_p2, Tuse_rs1_p3, Tuse_rs1_p4;
	reg Tuse_rt1_p1, Tuse_rt1_p2;
	
	always@(*) begin
		`DECODE
		
		oA1 = iinstr[25:21];
		oA2 = iinstr[20:16];
		
		if(add || addu || sub || subu || sll || srl || sra || sllv || srlv || srav) oA3 = iinstr[15:11]; // rd
		else if(slt || sltu || jalr || mfhi || mflo) oA3 = iinstr[15:11]; // rd
		else if(and_ || or_ || xor_ || nor_) oA3 = iinstr[15:11]; // rd
		else if(lb || lbu || lh || lhu || lw) oA3 = iinstr[20:16]; // rt
		else if(addi || addiu || andi || ori || xori || lui || slti || sltiu) oA3 = iinstr[20:16]; // rt
		else if(jal) oA3 = 5'h1f; // 0x1f
		else oA3 = 5'b0;
		
		Tuse_rs0 = beq || bne || blez || bgtz || bltz || bgez || jr || jalr;
		Tuse_rs1_p1 = add || addu || sub || subu || sllv || srlv || srav || and_ || or_ || xor_ || nor_;
		Tuse_rs1_p2 = mult || multu || div || divu || addi || addiu || andi || ori || xori || lui;
		Tuse_rs1_p3 = slt || slti || sltiu || sltu || mthi || mtlo;
		Tuse_rs1_p4 = lb || lbu || lh || lhu || lw || sb || sh || sw;
		Tuse_rs1 = Tuse_rs1_p1 || Tuse_rs1_p2 || Tuse_rs1_p3 || Tuse_rs1_p4;
		Tuse_rt0 = beq || bne;
		Tuse_rt1_p1 = add || addu || sub || subu || sllv || srlv || srav || and_ || or_ || xor_ || nor_;
		Tuse_rt1_p2 = mult || multu || div || divu || slt || sltu || sll || srl || sra;
		Tuse_rt1 = Tuse_rt1_p1 || Tuse_rt1_p2;
		Tuse_rt2 = sb || sh || sw;
		
		if(add || addu || sub || subu || sll || srl || sra || sllv || srlv || srav) Tnew = `TNEW_ALU;
		else if(slt || sltu || and_ || or_ || xor_ || nor_) Tnew = `TNEW_ALU;
		else if(addi || addiu || andi || ori || xori || lui || slti || sltiu) Tnew = `TNEW_ALU;
		else if(lb || lbu || lh || lhu || lw) Tnew = `TNEW_DM;
		else if(jal || jalr) Tnew = `TNEW_PC;
		else if(mfhi || mflo) Tnew = `TNEW_MD;
		else Tnew = `TNEW_NULL;
		
		oMDD = mult || multu || div || divu || mfhi || mflo || mthi || mtlo;
	end
	
endmodule

module ATREG(
		input [4:0] iA1,
		input [4:0] iA2,
		input [4:0] iA3,
		input iTuse_rs0,
		input iTuse_rs1,
		input iTuse_rt0,
		input iTuse_rt1,
		input iTuse_rt2,
		input [2:0] iTnew,
		input clk,
		input reset,
		output reg [4:0] oA1,
		output reg [4:0] oA2,
		output reg [4:0] oA3,
		output reg oTuse_rs0,
		output reg oTuse_rs1,
		output reg oTuse_rt0,
		output reg oTuse_rt1,
		output reg oTuse_rt2,
		output reg [2:0] oTnew
	 );
	
	always@(posedge clk) begin
		if(reset) begin
			oA1 <= 4'b0;
			oA2 <= 4'b0;
			oA3 <= 4'b0;
			oTuse_rs0 <= 0;
			oTuse_rs1 <= 0;
			oTuse_rt0 <= 0;
			oTuse_rt1 <= 0;
			oTuse_rt2 <= 0;
			oTnew <= 3'b0;
		end
		else begin
			oA1 <= iA1;
			oA2 <= iA2;
			oA3 <= iA3;
			oTuse_rs0 <= iTuse_rs0;
			oTuse_rs1 <= iTuse_rs1;
			oTuse_rt0 <= iTuse_rt0;
			oTuse_rt1 <= iTuse_rt1;
			oTuse_rt2 <= iTuse_rt2;
			oTnew <= iTnew;
		end
	end
	
endmodule

module STALL(
		input [4:0] iA1,
		input [4:0] iA2,
		input [4:0] iA3E,
		input [2:0] iTnewE,
		input [4:0] iA3M,
		input [2:0] iTnewM,
		input iMDD,
		input iMDbusy,
		input iTuse_rs0,
		input iTuse_rs1,
		input iTuse_rt0,
		input iTuse_rt1,
		input iTuse_rt2,
		output oPC_en,
		output oREGD_en,
		output oREGE_clear
	 );
	
	reg stall;
	
	reg stall_rs_E1, stall_rs_E2, stall_rs_E3, stall_rs_E4, stall_rs_M, stall_rs;
	reg stall_rt_E1, stall_rt_E2, stall_rt_E3, stall_rt_E4, stall_rt_M, stall_rt;
	reg stall_MD;
	
	always@(*) begin
		stall_rs_E1 = iTuse_rs0 && (iTnewE == `TNEW_ALU) && (iA3E == iA1) && ~(iA1 == 5'b0);
		stall_rs_E2 = iTuse_rs0 && (iTnewE == `TNEW_DM) && (iA3E == iA1) && ~(iA1 == 5'b0);
		stall_rs_E3 = iTuse_rs1 && (iTnewE == `TNEW_DM) && (iA3E == iA1) && ~(iA1 == 5'b0);
		stall_rs_E4 = iTuse_rs0 && (iTnewE == `TNEW_MD) && (iA3E == iA1) && ~(iA1 == 5'b0);
		stall_rs_M = iTuse_rs0 && (iTnewM == `TNEW_DM) && (iA3M == iA1) && ~(iA1 == 5'b0);
		stall_rs = stall_rs_E1 || stall_rs_E2 || stall_rs_E3 || stall_rs_E4 || stall_rs_M;
		
		stall_rt_E1 = iTuse_rt0 && (iTnewE == `TNEW_ALU) && (iA3E == iA2) && ~(iA2 == 5'b0);
		stall_rt_E2 = iTuse_rt0 && (iTnewE == `TNEW_DM) && (iA3E == iA2) && ~(iA2 == 5'b0);
		stall_rt_E3 = iTuse_rt1 && (iTnewE == `TNEW_DM) && (iA3E == iA2) && ~(iA2 == 5'b0);
		stall_rt_E4 = iTuse_rt0 && (iTnewE == `TNEW_MD) && (iA3E == iA2) && ~(iA2 == 5'b0);
		stall_rt_M = iTuse_rt0 && (iTnewM == `TNEW_DM) && (iA3M == iA2) && ~(iA2 == 5'b0);
		stall_rt = stall_rt_E1 || stall_rt_E2 || stall_rt_E3 || stall_rt_E4 || stall_rt_M;
		
		stall_MD = iMDD && iMDbusy;
		
		stall = stall_rs || stall_rt || stall_MD;
	end
	
	assign oPC_en = ~stall;
	assign oREGD_en = ~stall;
	assign oREGE_clear = stall;
	
endmodule

module BYPASSCTRL(
		input [4:0] iA3M,
		input [2:0] iTnewM,
		input [4:0] iA3W,
		input [2:0] iTnewW,
		input [4:0] iA1D,
		input [4:0] iA1E,
		input [4:0] iA2D,
		input [4:0] iA2E,
		input [4:0] iA2M,
		output reg [2:0] oMFGRFo1,
		output reg [2:0] oMFGRFo2,
		output reg [2:0] oMFALUi1,
		output reg [2:0] oMFALUi2,
		output reg oMFDMin
	 );
	
	always@(*) begin
		// MFGRFo1
		if(iA1D == 5'b0) oMFGRFo1 = `MF_NONE2D;
		else begin
			if(iA1D == iA3M) begin
				if(iTnewM == `TNEW_ALU) oMFGRFo1 = `MF_M2D;
				else if(iTnewM == `TNEW_PC) oMFGRFo1 = `MF_MPC2D;
				else if(iTnewM == `TNEW_MD) oMFGRFo1 = `MF_MMD2D;
				else oMFGRFo1 = `MF_NONE2D; // default, logically not necessary
			end
			else if(iA1D == iA3W) begin
				if(iTnewW == `TNEW_ALU) oMFGRFo1 = `MF_W2D;
				else if(iTnewW == `TNEW_PC) oMFGRFo1 = `MF_W2D;
				else if(iTnewW == `TNEW_DM) oMFGRFo1 = `MF_W2D; 
				else if(iTnewW == `TNEW_MD) oMFGRFo1 = `MF_W2D; // actually the same case
				else oMFGRFo1 = `MF_NONE2D; // default, logically not necessary
			end
			else oMFGRFo1 = `MF_NONE2D;
		end
		
		// MFGRFo2
		if(iA2D == 5'b0) oMFGRFo2 = `MF_NONE2D;
		else begin
			if(iA2D == iA3M) begin
				if(iTnewM == `TNEW_ALU) oMFGRFo2 = `MF_M2D;
				else if(iTnewM == `TNEW_PC) oMFGRFo2 = `MF_MPC2D;
				else if(iTnewM == `TNEW_MD) oMFGRFo2 = `MF_MMD2D;
				else oMFGRFo2 = `MF_NONE2D; // default, logically not necessary
			end
			else if(iA2D == iA3W) begin
				if(iTnewW == `TNEW_ALU) oMFGRFo2 = `MF_W2D;
				else if(iTnewW == `TNEW_PC) oMFGRFo2 = `MF_W2D;
				else if(iTnewW == `TNEW_DM) oMFGRFo2 = `MF_W2D; 
				else if(iTnewW == `TNEW_MD) oMFGRFo2 = `MF_W2D; // actually the same case
				else oMFGRFo2 = `MF_NONE2D; // default, logically not necessary
			end
			else oMFGRFo2 = `MF_NONE2D;
		end
		
		// MFALUi1
		if(iA1E == 5'b0) oMFALUi1 = `MF_NONE2E;
		else begin
			if(iA1E == iA3M) begin
				if(iTnewM == `TNEW_ALU) oMFALUi1 = `MF_M2E;
				else if(iTnewM == `TNEW_PC) oMFALUi1 = `MF_MPC2E;
				else if(iTnewM == `TNEW_MD) oMFALUi1 = `MF_MMD2E;
				else oMFALUi1 = `MF_NONE2E; // default, logically not necessary
			end
			else if(iA1E == iA3W) begin
				if(iTnewW == `TNEW_ALU) oMFALUi1 = `MF_W2E;
				else if(iTnewW == `TNEW_PC) oMFALUi1 = `MF_W2E;
				else if(iTnewW == `TNEW_DM) oMFALUi1 = `MF_W2E; 
				else if(iTnewW == `TNEW_MD) oMFALUi1 = `MF_W2E; // actually the same case
				else oMFALUi1 = `MF_NONE2E; // default, logically not necessary
			end
			else oMFALUi1 = `MF_NONE2E;
		end
		
		// MFALUi2
		if(iA2E == 5'b0) oMFALUi2 = `MF_NONE2E;
		else begin
			if(iA2E == iA3M) begin
				if(iTnewM == `TNEW_ALU) oMFALUi2 = `MF_M2E;
				else if(iTnewM == `TNEW_PC) oMFALUi2 = `MF_MPC2E;
				else if(iTnewM == `TNEW_MD) oMFALUi2 = `MF_MMD2E;
				else oMFALUi2 = `MF_NONE2E; // default, logically not necessary
			end
			else if(iA2E == iA3W) begin
				if(iTnewW == `TNEW_ALU) oMFALUi2 = `MF_W2E;
				else if(iTnewW == `TNEW_PC) oMFALUi2 = `MF_W2E;
				else if(iTnewW == `TNEW_DM) oMFALUi2 = `MF_W2E; 
				else if(iTnewW == `TNEW_MD) oMFALUi2 = `MF_W2E; // actually the same case
				else oMFALUi2 = `MF_NONE2E; // default, logically not necessary
			end
			else oMFALUi2 = `MF_NONE2E;
		end
		
		// MFDMin
		if(iA2M == 5'b0) oMFDMin = `MF_NONE2M;
		else begin
			if(iA2M == iA3W) begin
				if(iTnewW == `TNEW_ALU) oMFDMin = `MF_W2M;
				else if(iTnewW == `TNEW_DM) oMFDMin = `MF_W2M;
				else if(iTnewW == `TNEW_PC) oMFDMin = `MF_W2M;
				else if(iTnewW == `TNEW_MD) oMFDMin = `MF_W2M;
				else oMFDMin = `MF_NONE2M;
			end
			else oMFDMin = `MF_NONE2M;
		end
		
	end
	
endmodule
