`timescale 1ns / 1ps

module SEGF(
		// controller
		input [2:0] iNPC_sel,
		// hazard
		input iNPC_en,
		// others
		input [31:0] iinstrD,
		input [31:0] irs,
		input [31:0] iEPC,
		input clk,
		input reset,
		input resetpipe,
		input INT,
		output [31:0] oinstr,
		output [31:0] oPC8,
		output AdEL
    );

	wire [31:0] wNPC, wPC;
	
	PC _PC(
		.iNPC(wNPC), .clk(clk), .en(iNPC_en), .reset(reset), .oPC(wPC)
	);
	
	NPC _NPC(
		.iPC(wPC), .iinstr(iinstrD), .irs(irs), .iNPC_sel(iNPC_sel), .oNPC(wNPC), .oPC8(oPC8),
		.iEPC(iEPC), .INT(INT)
	);
	
	IM _IM(
		.iPC(wPC[13:2]), .oinstr(oinstr)
	);
	
	ADELF _AdELF(
		.iPC(wPC), .oEXC(AdEL)
	);
	
endmodule

module SEGD(
		// controller
		input [2:0] iEXT_op,
		input iGRF_WE,
		output [7:0] oCMPout,
		// hazard
		input iREGD_en,
		input [2:0] iFGRFo1_sel,
		input [2:0] iFGRFo2_sel,
		// others
		input [31:0] iinstr,
		input [31:0] iPC8,
		input [31:0] iPC8M, // bypass
		input [31:0] iPC8W, // display
		input clk,
		input reset,
		input resetpipe,
		input [31:0] iGRF_Data,
		input [4:0] iGRF_WD,
		input [31:0] iFM2D,
		input [31:0] iFMMD2D,
		input [31:0] iFW2D,
		output [31:0] oinstr,
		output [31:0] oGRFo1,
		output [31:0] oGRFo2,
		output [31:0] oPC8,
		output [31:0] oEXTout
    );

	wire [31:0] winstr, wGRFd1, wGRFd2, wGRFo1, wGRFo2;
	
	assign oinstr = winstr;
	assign oGRFo1 = wGRFo1;
	assign oGRFo2 = wGRFo2;

	REGD _REGD(
		.iinstr(iinstr), .iPC8(iPC8), .clk(clk), .en(iREGD_en), .reset(resetpipe), .oinstr(winstr), 
		.oPC8(oPC8)
	);
	
	GRF _GRF(
		.iA1_Addr(winstr[25:21]), .iA2_Addr(winstr[20:16]), .iA3_Addr(iGRF_WD), .iWD(iGRF_Data), 
		.iPC8(iPC8W), .clk(clk), .iWE(iGRF_WE), .reset(reset), .oA1_Data(wGRFd1), .oA2_Data(wGRFd2)
	);
	
	EXT _EXT(
		.i16(winstr[15:0]), .iop(iEXT_op), .o32(oEXTout)
	);
	
	// Due to the existance of internal bypass in RF, .in4(iFW2D) is actually not necessary.
	// Setting the F2D bypass is to make bypass logic in different segments identical.
	MUX_8_32bits _MFGRFo1(
		.in0(wGRFd1), .in1(iFM2D), .in2(iPC8M), .in3(iFMMD2D), .in4(iFW2D), .isel(iFGRFo1_sel), 
		.out(wGRFo1)
	);
	
	MUX_8_32bits _MFGRFo2(
		.in0(wGRFd2), .in1(iFM2D), .in2(iPC8M), .in3(iFMMD2D), .in4(iFW2D), .isel(iFGRFo2_sel), 
		.out(wGRFo2)
	);
	
	CMP _CMP(
		.iA1(wGRFo1), .iA2(wGRFo2), .oresult(oCMPout)
	);

endmodule

module SEGE(
		// controller
		input iALU_src,
		input iMD_sel,
		input [1:0] iGRF_WD,
		input [4:0] iALU_op,
		// hazard
		input iREGE_clear,
		input [2:0] iFALUi1_sel,
		input [2:0] iFALUi2_sel,
		output oMDoccupy,
		// others
		input [31:0] iinstr,
		input [31:0] iGRFo1,
		input [31:0] iGRFo2,
		input [31:0] iPC8,
		input [31:0] iPC8M,
		input [31:0] iEXTout,
		input [31:0] iFM2E,
		input [31:0] iFW2E,
		input [31:0] iFMMD2E,
		input [7:0] iCMPout,
		input clk,
		input reset,
		input resetpipe,
		input disab,
		output [31:0] oinstr,
		output [31:0] oALUout,
		output [31:0] oALUi2,
		output [31:0] oPC8,
		output [31:0] oMDout,
		output [7:0] oCMPout,
		output [4:0] oGRF_WD,
		output overflow
    );

	wire [31:0] winstr, wGRFo1, wGRFo2, wEXTout;
	wire [31:0] wALUi1, wALUi2, wALUsrc2;
	wire [31:0] wHI, wLO;
	wire [4:0] wconst1f;
	reg [4:0] rconst1f;
	
	initial rconst1f = 5'h1f;
	
	assign wconst1f = rconst1f;
	assign oinstr = winstr;
	assign oALUi2 = wALUi2;

	REGE _REGE(
		.iinstr(iinstr), .iGRFo1(iGRFo1), .iGRFo2(iGRFo2), .iPC8(iPC8), .iEXTout(iEXTout), 
		.iCMPout(iCMPout), .clk(clk), .clear(iREGE_clear), .reset(resetpipe), .oinstr(winstr), 
		.oGRFo1(wGRFo1), .oGRFo2(wGRFo2), .oPC8(oPC8), .oEXTout(wEXTout), .oCMPout(oCMPout)
	);
	
	MUX_8_32bits _MFALUi1(
		.in0(wGRFo1), .in1(iFM2E), .in2(iPC8M), .in3(iFMMD2E), .in4(iFW2E), .isel(iFALUi1_sel), 
		.out(wALUi1)
	);
	
	MUX_8_32bits _MFALUi2(
		.in0(wGRFo2), .in1(iFM2E), .in2(iPC8M), .in3(iFMMD2E), .in4(iFW2E), .isel(iFALUi2_sel), 
		.out(wALUi2)
	);
	
	MUX_2_32bits _MALUsrc(
		.in0(wALUi2), .in1(wEXTout), .isel(iALU_src), .out(wALUsrc2)
	);
	
	ALU _ALU(
		.iA1(wALUi1), .iA2(wALUsrc2), .isa(winstr[10:6]), .iop(iALU_op), .oresult(oALUout),
		.overflow(overflow)
	);
	
	MD _MD(
		.iinstr(winstr), .iA1(wALUi1), .iA2(wALUsrc2), .clk(clk), .reset(reset), .oHI(wHI), 
		.oLO(wLO), .ooccupy(oMDoccupy), .disab(disab)
	);
	
	MUX_2_32bits _MMDout(
		.in0(wHI), .in1(wLO), .isel(iMD_sel), .out(oMDout)
	);
	
	MUX_4_5bits _MGRFWD(
		.in0(winstr[15:11]), .in1(winstr[20:16]), .in2(wconst1f), .isel(iGRF_WD), .out(oGRF_WD)
	);

endmodule

module SEGM(
		// controller
		input [3:0] iDMctrl,
		// hazard
		input iFDMin_sel,
		// others
		input [31:0] iinstr,
		input [31:0] iALUout,
		input [31:0] iALUi2,
		input [31:0] iPC8,
		input [31:0] iMDout,
		input [31:0] iFW2M,
		input [7:0] iCMPout,
		input [4:0] iGRF_WD,
		input clk,
		input reset,
		input resetpipe,
		input INT,
		output [31:0] oinstr,
		output [31:0] oALUout,
		output [31:0] oDMout,
		output [31:0] oPC8,
		output [31:0] oMDout,
		output [31:0] oRTM,
		output [7:0] oCMPout,
		output [4:0] oGRF_WD,
		input [31:0] iDEV_Data,
		input [31:0] iCP0_Data,
		output AdEL,
		output AdES,
		output [31:0] oDEV_Data,
		output [3:0] oDEV_BE
    );
	 
	wire [31:0] winstr, wALUout, wALUi2, wPC8, wDMin, wDM_WD;
	wire [31:0] wDM_Data;
	wire [31:0] wAddr;
	wire [3:0] wBE;
	
	assign oinstr = winstr;
	assign oALUout = wALUout;
	assign oPC8 = wPC8;
	assign oDEV_Data = wDM_WD;
	assign oRTM = wDMin;

	REGM _REGM(
		.iinstr(iinstr), .iALUout(iALUout), .iALUi2(iALUi2), .iPC8(iPC8), .iMDout(iMDout), 
		.iCMPout(iCMPout), .iGRF_WD(iGRF_WD), .clk(clk), .reset(resetpipe), .oinstr(winstr), 
		.oALUout(wALUout), .oALUi2(wALUi2), .oPC8(wPC8), .oMDout(oMDout), .oCMPout(oCMPout), 
		.oGRF_WD(oGRF_WD)
	);
	
	MUX_2_32bits _MFDMin(
		.in0(wALUi2), .in1(iFW2M), .isel(iFDMin_sel), .out(wDMin)
	);
	
	DMF _DMF(
		.iAddr(wALUout), .iData(wDMin), .ictrl(iDMctrl), .oAddr(wAddr), .oData(wDM_WD), .oBE(wBE),
		.oEXC(AdES), .oDEV_BE(oDEV_BE), .INT(INT)
	);
	
	DM _DM(
		.iAddr(wAddr[13:2]), .iData(wDM_WD), .iPC8(wPC8), .iDM_BE(wBE), .clk(clk), .reset(reset), 
		.out(wDM_Data)
	);
	
	DMB _DMB(
		.iAddr(wALUout), .iData(wDM_Data), .ictrl(iDMctrl), .out(oDMout),
		.iDEV_Data(iDEV_Data), .iCP0_Data(iCP0_Data), .oEXC(AdEL)
	);

endmodule

module SEGW(
		// controller
		input [1:0] iGRF_WS,
		output [7:0] oCMPout,
		// others
		input [31:0] iinstr,
		input [31:0] iALUout,
		input [31:0] iDMout,
		input [31:0] iPC8,
		input [31:0] iMDout,
		input [7:0] iCMPout,
		input [4:0] iGRF_WD,
		input clk,
		input reset,
		input resetpipe,
		output [31:0] oinstr,
		output [31:0] oGRF_WS,
		output [31:0] oPC8,
		output [4:0] oGRF_WD
    );

	wire [31:0] wALUout, wDMout, wPC8, wMDout;
	
	assign oPC8 = wPC8;

	REGW _REGW(
		.iinstr(iinstr), .iALUout(iALUout), .iDMout(iDMout), .iPC8(iPC8), .iMDout(iMDout), 
		.iCMPout(iCMPout), .iGRF_WD(iGRF_WD), .clk(clk), .reset(resetpipe), .oinstr(oinstr), 
		.oALUout(wALUout), .oDMout(wDMout), .oPC8(wPC8), .oMDout(wMDout), .oCMPout(oCMPout), 
		.oGRF_WD(oGRF_WD)
	);
	
	MUX_4_32bits _MGRFWS(
		.in0(wALUout), .in1(wDMout), .in2(wPC8), .in3(wMDout), .isel(iGRF_WS), .out(oGRF_WS)
	);

endmodule
