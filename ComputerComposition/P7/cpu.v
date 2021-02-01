`timescale 1ns / 1ps

`include "define.v"
module CPU(
		input clk,
		input reset,
		input [31:0] iDEV_Data,
		input [5:0] iINTRQ,
		output [31:0] oDEV_Data,
		output [31:0] oDEV_Addr,
		output [3:0] oDEV_BE,
		output [31:0] runtime_PC
    );

	wire [31:0] instrF, instrD, instrE, instrM, instrW;
	wire [31:0] PC8F, PC8D, PC8E, PC8M, PC8W;
	wire [31:0] GRFo1, GRFo2, GRF_Data;
	wire [31:0] EXTout;
	wire [31:0] ALUoutE, rtE, MDoutE;
	wire [31:0] ALUoutM, MDoutM, DMout;
	wire [7:0] CMPoutD, CMPoutE, CMPoutM, CMPoutW;
	wire [4:0] GRF_WDE, GRF_WDM, GRF_WDW, ALUop;
	wire [3:0] DMctrl;
	wire [2:0] NPC_sel, EXTop, FGRFo1_sel, FGRFo2_sel, FALUi1_sel, FALUi2_sel;
	wire [1:0] GRF_WD, GRF_WS;
	wire NPC_en, GRF_WE, REGD_en, ALU_src, MD_sel, REGE_clear, MDbusy, FDMin_sel;
	
	wire [31:0] PCF, PCD, PCE, PCM;
	wire [31:0] CP0_Data, EPC, rtM;
	wire [4:0] ExcCodeE, ExcCodeM;
	wire AdELF, AdELM, AdESM, EXCD, EXCE, EXCM;
	wire isBJD, isBJE, isBJM, RID, overflow, wINTRQ;
	wire LOADtype, SAVEtype, CP0_WE, REGD_enPLUS, EXLclr;
	wire resetpipe, resetpipeD;
	wire MTC0E, MTC0M;
	
	assign PCF = PC8F - 32'h8;
	assign resetpipe = reset || wINTRQ;
	assign resetpipeD = (reset || wINTRQ || (NPC_sel == `NPC_ERET)) && REGD_enPLUS;
	assign REGD_enPLUS = REGD_en || wINTRQ;
	assign NPC_enPLUS = NPC_en || wINTRQ;
	// if fail to catch some of the interrupts, try to change the segment to clear EXL
	assign EXLclr = (instrM == 32'h42000018);
	
	assign oDEV_Addr = ALUoutM;
	assign runtime_PC = PCM;
	
	assign MTC0E = (instrE[31:26] == 6'b010000) && (instrE[25:21] == 5'b00100);
	assign MTC0M = (instrM[31:26] == 6'b010000) && (instrM[25:21] == 5'b00100);
	
	SEGF _SEGF(
		.iNPC_sel(NPC_sel), .iNPC_en(NPC_enPLUS), .iinstrD(instrD), .irs(GRFo1), .clk(clk), .reset(reset), 
		.oinstr(instrF), .oPC8(PC8F),
		.AdEL(AdELF), .iEPC(EPC), .resetpipe(resetpipe), .INT(wINTRQ)
	);
	
	CTRLD _CTRLD(
		.iinstr(instrD), .iCMPout(CMPoutD), .oEXTop(EXTop), .oNPC_sel(NPC_sel), 
		.isBJ(isBJD), .RI(RID)
	);
	
	EXCD _EXCD(
		.iEXC(AdELF), .iPC(PCF), .clk(clk), .reset(resetpipeD), .enable(REGD_enPLUS), .oEXC(EXCD), .oPC(PCD)
	);
	
	SEGD _SEGD(
		.iEXT_op(EXTop), .iGRF_WE(GRF_WE), .oCMPout(CMPoutD), .iREGD_en(REGD_enPLUS), .iFGRFo1_sel(FGRFo1_sel), 
		.iFGRFo2_sel(FGRFo2_sel), .iinstr(instrF), .iPC8(PC8F), .iPC8M(PC8M), .iPC8W(PC8W), .clk(clk), 
		.reset(reset), .iGRF_Data(GRF_Data), .iGRF_WD(GRF_WDW), .iFM2D(ALUoutM), .iFMMD2D(MDoutM), 
		.iFW2D(GRF_Data), .oinstr(instrD), .oGRFo1(GRFo1), .oGRFo2(GRFo2), .oPC8(PC8D), .oEXTout(EXTout),
		.resetpipe(resetpipeD)
	);
	
	CTRLE _CTRLE(
		.iinstr(instrE), .oALUop(ALUop), .oALUsrc(ALU_src), .oMD_sel(MD_sel), .oGRF_WD(GRF_WD),
		.LOADtype(LOADtype), .SAVEtype(SAVEtype)
	);
	
	EXCE _EXCE(
		.iEXC(RID), .iisBJ(isBJD), .iPC(PCD), .iEXCD(EXCD), .clk(clk), .reset(resetpipe), .clear(REGE_clear),
		.oEXC(EXCE), .oisBJ(isBJE), .oPC(PCE), .oExcCode(ExcCodeE)
	);
	 
	SEGE _SEGE(
		.iALU_src(ALU_src), .iMD_sel(MD_sel), .iGRF_WD(GRF_WD), .iALU_op(ALUop), .iREGE_clear(REGE_clear), 
		.iFALUi1_sel(FALUi1_sel), .iFALUi2_sel(FALUi2_sel), .oMDoccupy(MDbusy), .iinstr(instrD), 
		.iGRFo1(GRFo1), .iGRFo2(GRFo2), .iPC8(PC8D), .iPC8M(PC8M), .iEXTout(EXTout), .iFM2E(ALUoutM), 
		.iFW2E(GRF_Data), .iFMMD2E(MDoutM), .iCMPout(CMPoutD), .clk(clk), .reset(reset), .oinstr(instrE), 
		.oALUout(ALUoutE), .oALUi2(rtE), .oPC8(PC8E), .oMDout(MDoutE), .oCMPout(CMPoutE), .oGRF_WD(GRF_WDE),
		.overflow(overflow), .resetpipe(resetpipe), .disab(wINTRQ)
	);
	
	CTRLM _CTRLM(
		.iinstr(instrM), .octrl(DMctrl), .CP0_WE(CP0_WE)
	);
	
	EXCM _EXCM(
		.ioverflow(overflow), .iLOADtype(LOADtype), .iSAVEtype(SAVEtype), .iEXCE(EXCE), .iisBJ(isBJE), 
		.iExcCode(ExcCodeE), .iPC(PCE), .clk(clk), .reset(resetpipe), .oEXC(EXCM), .oisBJ(isBJM), 
		.oExcCode(ExcCodeM), .oPC(PCM)
	);

	SEGM _SEGM(
		.iDMctrl(DMctrl), .iFDMin_sel(FDMin_sel), .iinstr(instrE), .iALUout(ALUoutE), .iALUi2(rtE), 
		.iPC8(PC8E), .iMDout(MDoutE), .iFW2M(GRF_Data), .iCMPout(CMPoutE), .iGRF_WD(GRF_WDE), 
		.clk(clk), .reset(reset), .oinstr(instrM), .oALUout(ALUoutM), .oDMout(DMout), .oPC8(PC8M),
		.oMDout(MDoutM), .oCMPout(CMPoutM), .oGRF_WD(GRF_WDM), .oRTM(rtM),
		.AdEL(AdELM), .AdES(AdESM), .iDEV_Data(iDEV_Data), .iCP0_Data(CP0_Data), .resetpipe(resetpipe),
		.oDEV_Data(oDEV_Data), .oDEV_BE(oDEV_BE), .INT(wINTRQ)
	);
	
	CP0 _CP0(
		.iAdES(AdESM), .iAdEL(AdELM), .iINTRQ(iINTRQ), .iEXCM(EXCM), .iisBJ(isBJM), 
		.iExcCode(ExcCodeM), .iPC(PCM), .iData(rtM), .iAddr(instrM[15:11]), .iWE(CP0_WE), 
		.iEXLclr(EXLclr), .clk(clk), .reset(reset), .oData(CP0_Data), .oEPC(EPC), .oINTRQ(wINTRQ),
		.iPCE(PCE), .iPCD(PCD), .iPCF(PCF), .stall(REGE_clear)
	);
	
	CTRLW _CTRLW(
		.iinstr(instrW), .iCMPout(CMPoutW), .oGRF_WS(GRF_WS), .oGRF_WE(GRF_WE)
	);
	
	SEGW _SEGW(
		.iGRF_WS(GRF_WS), .oCMPout(CMPoutW), .iinstr(instrM), .iALUout(ALUoutM), .iDMout(DMout), 
		.iPC8(PC8M), .iMDout(MDoutM), .iCMPout(CMPoutM), .iGRF_WD(GRF_WDM), .clk(clk), .reset(reset), 
		.oinstr(instrW), .oGRF_WS(GRF_Data), .oPC8(PC8W), .oGRF_WD(GRF_WDW), .resetpipe(resetpipe)
	);
	
	HAZARD _HAZARD(
		.iinstr(instrD), .clk(clk), .reset(resetpipe), .iMDbusy(MDbusy), .oPC_en(NPC_en), .oREGD_en(REGD_en), 
		.oREGE_clear(REGE_clear), .oMFGRFo1(FGRFo1_sel), .oMFGRFo2(FGRFo2_sel), .oMFALUi1(FALUi1_sel), 
		.oMFALUi2(FALUi2_sel), .oMFDMin(FDMin_sel), .iMTC0E(MTC0E), .iMTC0M(MTC0M), .resetpipe(resetpipe)
	);

endmodule
