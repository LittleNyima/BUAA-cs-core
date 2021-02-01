`timescale 1ns / 1ps

module mips(
		input clk,
		input reset
    );

	wire [31:0] instrF, instrD, instrE, instrM, instrW;
	wire [31:0] PC8F, PC8D, PC8E, PC8M, PC8W;
	wire [31:0] GRFo1, GRFo2, GRF_Data;
	wire [31:0] EXTout;
	wire [31:0] ALUoutE, rtE, MDoutE;
	wire [31:0] ALUoutM, MDoutM, DMout;
	wire [7:0] CMPoutD, CMPoutE, CMPoutM, CMPoutW;
	wire [4:0] GRF_WDE, GRF_WDM, GRF_WDW;
	wire [3:0] ALUop, DMctrl;
	wire [2:0] NPC_sel, EXTop, FGRFo1_sel, FGRFo2_sel, FALUi1_sel, FALUi2_sel;
	wire [1:0] GRF_WD, GRF_WS;
	wire NPC_en, GRF_WE, REGD_en, ALU_src, MD_sel, REGE_clear, MDbusy, FDMin_sel;
	
	//assign o = MDbusy;
	
	SEGF _SEGF(
		.iNPC_sel(NPC_sel), .iNPC_en(NPC_en), .iinstrD(instrD), .irs(GRFo1), .clk(clk), .reset(reset), 
		.oinstr(instrF), .oPC8(PC8F)
	);
	
	CTRLD _CTRLD(
		.iinstr(instrD), .iCMPout(CMPoutD), .oEXTop(EXTop), .oNPC_sel(NPC_sel)
	);
	
	SEGD _SEGD(
		.iEXT_op(EXTop), .iGRF_WE(GRF_WE), .oCMPout(CMPoutD), .iREGD_en(REGD_en), .iFGRFo1_sel(FGRFo1_sel), 
		.iFGRFo2_sel(FGRFo2_sel), .iinstr(instrF), .iPC8(PC8F), .iPC8M(PC8M), .iPC8W(PC8W), .clk(clk), 
		.reset(reset), .iGRF_Data(GRF_Data), .iGRF_WD(GRF_WDW), .iFM2D(ALUoutM), .iFMMD2D(MDoutM), 
		.iFW2D(GRF_Data), .oinstr(instrD), .oGRFo1(GRFo1), .oGRFo2(GRFo2), .oPC8(PC8D), .oEXTout(EXTout)
	);
	
	CTRLE _CTRLE(
		.iinstr(instrE), .oALUop(ALUop), .oALUsrc(ALU_src), .oMD_sel(MD_sel), .oGRF_WD(GRF_WD)
	);
	
	SEGE _SEGE(
		.iALU_src(ALU_src), .iMD_sel(MD_sel), .iGRF_WD(GRF_WD), .iALU_op(ALUop), .iREGE_clear(REGE_clear), 
		.iFALUi1_sel(FALUi1_sel), .iFALUi2_sel(FALUi2_sel), .oMDoccupy(MDbusy), .iinstr(instrD), 
		.iGRFo1(GRFo1), .iGRFo2(GRFo2), .iPC8(PC8D), .iPC8M(PC8M), .iEXTout(EXTout), .iFM2E(ALUoutM), 
		.iFW2E(GRF_Data), .iFMMD2E(MDoutM), .iCMPout(CMPoutD), .clk(clk), .reset(reset), .oinstr(instrE), 
		.oALUout(ALUoutE), .oALUi2(rtE), .oPC8(PC8E), .oMDout(MDoutE), .oCMPout(CMPoutE), .oGRF_WD(GRF_WDE)
	);
	
	CTRLM _CTRLM(
		.iinstr(instrM), .octrl(DMctrl)
	);

	SEGM _SEGM(
		.iDMctrl(DMctrl), .iFDMin_sel(FDMin_sel), .iinstr(instrE), .iALUout(ALUoutE), .iALUi2(rtE), 
		.iPC8(PC8E), .iMDout(MDoutE), .iFW2M(GRF_Data), .iCMPout(CMPoutE), .iGRF_WD(GRF_WDE), 
		.clk(clk), .reset(reset), .oinstr(instrM), .oALUout(ALUoutM), .oDMout(DMout), .oPC8(PC8M),
		.oMDout(MDoutM), .oCMPout(CMPoutM), .oGRF_WD(GRF_WDM)
	);
	
	CTRLW _CTRLW(
		.iinstr(instrW), .iCMPout(CMPoutW), .oGRF_WS(GRF_WS), .oGRF_WE(GRF_WE)
	);
	
	SEGW _SEGW(
		.iGRF_WS(GRF_WS), .oCMPout(CMPoutW), .iinstr(instrM), .iALUout(ALUoutM), .iDMout(DMout), 
		.iPC8(PC8M), .iMDout(MDoutM), .iCMPout(CMPoutM), .iGRF_WD(GRF_WDM), .clk(clk), .reset(reset), 
		.oinstr(instrW), .oGRF_WS(GRF_Data), .oPC8(PC8W), .oGRF_WD(GRF_WDW)
	);
	
	HAZARD _HAZARD(
		.iinstr(instrD), .clk(clk), .reset(reset), .iMDbusy(MDbusy), .oPC_en(NPC_en), .oREGD_en(REGD_en), 
		.oREGE_clear(REGE_clear), .oMFGRFo1(FGRFo1_sel), .oMFGRFo2(FGRFo2_sel), .oMFALUi1(FALUi1_sel), 
		.oMFALUi2(FALUi2_sel), .oMFDMin(FDMin_sel)
	);

endmodule
