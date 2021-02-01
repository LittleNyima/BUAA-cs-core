`timescale 1ns / 1ps

module mips(
		input clk,
		input reset
    );
	
	wire [31:0] instrF, instrD, instrE, instrM, instrW;
	wire [31:0] PC8F, PC8D, PC8E, PC8M, PC8W;
	wire [31:0] GRFo1, GRFo2, EXTout; // pass between D and E
	wire [31:0] ALUoutE, ALUi2; // pass between E and M
	wire [31:0] ALUoutM, DMout; // pass between M and W
	wire [31:0] GRF_Data;
	wire [4:0] GRF_WDE, GRF_WDM, GRF_WDW;
	wire [2:0] NPC_sel, ALU_op, EXTop;
	wire [1:0] FGRFo1_sel, FGRFo2_sel, FALUi1_sel, FALUi2_sel;
	wire [1:0] GRF_WS, GRF_WD;
	wire PC_en, REGD_en, REGE_clear, FDMin_sel;
	wire CMP_equal, GRF_WE, DM_WE, ALU_src;
	
	segmentF SEGF(.iNPC_sel(NPC_sel), .iNPC_en(PC_en), .iinstr(instrD), .irs(GRFo1), 
					  .clk(clk), .reset(reset), .oinstr(instrF), .oPC8(PC8F));
	
	CTRLD CTLD(.iinstr(instrD), .iCMP_equal(CMP_equal), .oEXT_op(EXTop), .oNPC_sel(NPC_sel));
	
	segmentD SEGD(.iEXT_op(EXTop), .iGRF_WE(GRF_WE), .oCMP_equal(CMP_equal), .iREGD_en(REGD_en), 
					  .iFGRFo1_sel(FGRFo1_sel), .iFGRFo2_sel(FGRFo2_sel), .iinstr(instrF), .iPC8(PC8F), 
					  .iPC8M(PC8M), .iPC8W(PC8W), .clk(clk), .reset(reset), .iGRF_Data(GRF_Data), 
					  .iGRF_WD(GRF_WDW), .iFM2D(ALUoutM), .iFW2D(GRF_Data), .oinstr(instrD),.oGRFo1(GRFo1), 
					  .oGRFo2(GRFo2), .oPC8(PC8D), .oEXTout(EXTout));
	
	CTRLE CTLE(.iinstr(instrE), .oALU_src(ALU_src), .oALU_op(ALU_op), .oGRF_WD(GRF_WD));
	
	segmentE SEGE(.iALU_src(ALU_src), .iGRF_WD(GRF_WD), .iALU_op(ALU_op), .iREGE_clear(REGE_clear), 
					  .iFALUi1_sel(FALUi1_sel), .iFALUi2_sel(FALUi2_sel), .iinstr(instrD), .iGRFo1(GRFo1), 
					  .iGRFo2(GRFo2), .iPC8(PC8D), .iPC8M(PC8M), .iEXTout(EXTout), .iFM2E(ALUoutM), 
					  .iFW2E(GRF_Data), .clk(clk), .reset(reset), .oinstr(instrE), .oALUout(ALUoutE), 
					  .oALUi2(ALUi2), .oPC8(PC8E), .oGRF_WD(GRF_WDE));
	
	CTRLM CTLM(.iinstr(instrM), .oDM_WE(DM_WE));
	
	segmentM SEGM(.iDM_WE(DM_WE), .iFDMin_sel(FDMin_sel), .iinstr(instrE), .iALUout(ALUoutE), 
					  .iALUi2(ALUi2), .iPC8(PC8E), .iGRF_WD(GRF_WDE), .iFW2M(GRF_Data), .clk(clk), 
					  .reset(reset), .oinstr(instrM), .oALUout(ALUoutM), .oDMout(DMout), .oPC8(PC8M), 
					  .oGRF_WD(GRF_WDM));
	
	CTRLW CTLW(.iinstr(instrW), .oGRF_WS(GRF_WS), .oGRF_WE(GRF_WE));
	
	segmentW SEGW(.iGRF_WS(GRF_WS), .iinstr(instrM), .iALUout(ALUoutM), .iDMout(DMout), .iPC8(PC8M), 
					  .iGRF_WD(GRF_WDM), .clk(clk), .reset(reset), .oinstr(instrW), .oGRF_WS(GRF_Data), 
					  .oPC8(PC8W), .oGRF_WD(GRF_WDW));
	
	HAZARD HZD(.iinstr(instrD), .clk(clk), .reset(reset), .oPC_en(PC_en), .oREGD_en(REGD_en), 
				  .oREGE_clear(REGE_clear), .oMFGRFo1(FGRFo1_sel), .oMFGRFo2(FGRFo2_sel), 
				  .oMFALUi1(FALUi1_sel), .oMFALUi2(FALUi2_sel), .oMFDMin(FDMin_sel));

endmodule
