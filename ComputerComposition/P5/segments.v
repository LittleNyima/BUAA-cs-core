`timescale 1ns / 1ps

module segmentF(
		// controller
		input [2:0] iNPC_sel,
		// harzard
		input iNPC_en,
		// others
		input [31:0] iinstr,
		input [31:0] irs,
		input clk,
		input reset,
		output [31:0] oinstr,
		output [31:0] oPC8
    );

	wire [31:0] wPC;
	wire [31:0] wNPC;
	
	IM FIM(.iPC(wPC[11:2]), .oinstr(oinstr));
	
	PC FPC(.iNPC(wNPC), .clk(clk), .en(iNPC_en), .reset(reset), .oPC(wPC));
	
	NPC FNPC(.iPC(wPC), .iinstr(iinstr), .irs(irs), .iNPC_sel(iNPC_sel), .oNPC(wNPC), .oPC8(oPC8));

endmodule

module segmentD(
		// controller
		input [2:0] iEXT_op,
		input iGRF_WE,
		output oCMP_equal,
		// hazard
		input iREGD_en,
		input [1:0] iFGRFo1_sel,
		input [1:0] iFGRFo2_sel,
		// others
		input [31:0] iinstr,
		input [31:0] iPC8,
		input [31:0] iPC8M,
		input [31:0] iPC8W,
		input clk,
		input reset,
		input [31:0] iGRF_Data,
		input [4:0] iGRF_WD,
		input [31:0] iFM2D,
		input [31:0] iFW2D,
		output [31:0] oinstr,
		output [31:0] oGRFo1,
		output [31:0] oGRFo2,
		output [31:0] oPC8,
		output [31:0] oEXTout
    );

	wire [31:0] wGRFo1;
	wire [31:0] wGRFo2;
	wire [31:0] wMFGRFo1;
	wire [31:0] wMFGRFo2;
	wire [31:0] winstr;
	
	REGD DREGD(.iinstr(iinstr), .iPC8(iPC8), .clk(clk), .en(iREGD_en), .reset(reset), .oinstr(winstr), .oPC8(oPC8));
	
	GRF DGRF(.iA1_Addr(winstr[25:21]), .iA2_Addr(winstr[20:16]), .iA3_Addr(iGRF_WD), .iWD(iGRF_Data), .iPC8(iPC8W), 
	         .clk(clk), .iWE(iGRF_WE), .reset(reset), .oA1_Data(wGRFo1), .oA2_Data(wGRFo2));
				
	EXT DEXT(.i16(winstr[15:0]), .iop(iEXT_op), .o32(oEXTout));
	
	MUX_4_32bits DMFGRFo1(.in0(iFM2D), .in1(iFW2D), .in2(iPC8M), .in3(wGRFo1), .isel(iFGRFo1_sel), .out(wMFGRFo1));
	
	MUX_4_32bits DMFGRFo2(.in0(iFM2D), .in1(iFW2D), .in2(iPC8M), .in3(wGRFo2), .isel(iFGRFo2_sel), .out(wMFGRFo2));
	
	CMP DCMP(.iA1(wMFGRFo1), .iA2(wMFGRFo2), .oequal(oCMP_equal));
	
	assign oinstr = winstr;
	assign oGRFo1 = wMFGRFo1;
	assign oGRFo2 = wMFGRFo2;

endmodule

module segmentE(
		// controller
		input iALU_src,
		input [1:0] iGRF_WD,
		input [2:0] iALU_op,
		// hazard
		input iREGE_clear,
		input [1:0] iFALUi1_sel,
		input [1:0] iFALUi2_sel,
		// others
		input [31:0] iinstr,
		input [31:0] iGRFo1,
		input [31:0] iGRFo2,
		input [31:0] iPC8,
		input [31:0] iPC8M,
		input [31:0] iEXTout,
		input [31:0] iFM2E,
		input [31:0] iFW2E,
		input clk,
		input reset,
		output [31:0] oinstr,
		output [31:0] oALUout,
		output [31:0] oALUi2,
		output [31:0] oPC8,
		output [4:0] oGRF_WD
    );

	reg [4:0] rconst1f;
	initial rconst1f = 5'h1f;
	
	wire [31:0] wGRFo1;
	wire [31:0] wGRFo2;
	wire [31:0] wEXTout;
	wire [31:0] wMFALUi1;
	wire [31:0] wMFALUi2;
	wire [31:0] wMALUi2;
	wire [4:0] wconst1f;
	wire [31:0] winstr;
	
	assign wconst1f = rconst1f;
	
	REGE EREGE(.iinstr(iinstr), .iGRFo1(iGRFo1), .iGRFo2(iGRFo2), .iPC8(iPC8), .iEXTout(iEXTout), 
	           .clk(clk), .clear(iREGE_clear), .reset(reset), .oinstr(winstr), .oGRFo1(wGRFo1), 
				  .oGRFo2(wGRFo2), .oPC8(oPC8), .oEXTout(wEXTout));

	MUX_4_32bits EMFALUi1(.in0(iFM2E), .in1(iFW2E), .in2(iPC8M), .in3(wGRFo1), .isel(iFALUi1_sel), .out(wMFALUi1));
	
	MUX_4_32bits EMFALUi2(.in0(iFM2E), .in1(iFW2E), .in2(iPC8M), .in3(wGRFo2), .isel(iFALUi2_sel), .out(wMFALUi2));
	
	MUX_4_5bits EMGRF_WD(.in0(winstr[20:16]), .in1(winstr[15:11]), .in2(wconst1f), .isel(iGRF_WD), .out(oGRF_WD));
	
	MUX_2_32bits EMALUi2(.in0(wMFALUi2), .in1(wEXTout), .isel(iALU_src), .out(wMALUi2));
	
	ALU EALU(.iA1(wMFALUi1), .iA2(wMALUi2), .iop(iALU_op), .out(oALUout));
	
	assign oinstr = winstr;
	assign oALUi2 = wMFALUi2;

endmodule

module segmentM(
		// controller
		input iDM_WE,
		// hazard
		input iFDMin_sel,
		// others
		input [31:0] iinstr,
		input [31:0] iALUout,
		input [31:0] iALUi2,
		input [31:0] iPC8,
		input [4:0] iGRF_WD,
		input [31:0] iFW2M,
		input clk,
		input reset,
		output [31:0] oinstr,
		output [31:0] oALUout,
		output [31:0] oDMout,
		output [31:0] oPC8,
		output [4:0] oGRF_WD
    );
	
	wire [31:0] wALUout;
	wire [31:0] wALUi2;
	wire [31:0] wPC8;
	wire [31:0] wDMin;
	
	REGM MREGM(.iinstr(iinstr), .iALUout(iALUout), .iALUi2(iALUi2), .iPC8(iPC8), .iGRF_WD(iGRF_WD), 
				  .clk(clk), .reset(reset), .oinstr(oinstr), .oALUout(wALUout), .oALUi2(wALUi2), 
				  .oPC8(wPC8), .oGRF_WD(oGRF_WD));
	
	MUX_2_32bits MMFDMin(.in0(iFW2M), .in1(wALUi2), .isel(iFDMin_sel), .out(wDMin));
	
	DM MDM(.iAddr(wALUout[11:2]), .iData(wDMin), .iPC8(wPC8), .iDM_WE(iDM_WE), .clk(clk), .reset(reset), 
			 .out(oDMout));
	
	assign oALUout = wALUout;
	assign oPC8 = wPC8;

endmodule

module segmentW(
		// controller
		input [1:0] iGRF_WS,
		// others
		input [31:0] iinstr,
		input [31:0] iALUout,
		input [31:0] iDMout,
		input [31:0] iPC8,
		input [4:0] iGRF_WD,
		input clk,
		input reset,
		output [31:0] oinstr,
		output [31:0] oGRF_WS,
		output [31:0] oPC8,
		output [4:0] oGRF_WD
	 );
	
	wire [31:0] wALUout;
	wire [31:0] wDMout;
	wire [31:0] wPC8;
	
	REGW WREGW(.iinstr(iinstr), .iALUout(iALUout), .iDMout(iDMout), .iPC8(iPC8), .iGRF_WD(iGRF_WD), .clk(clk), 
				  .reset(reset), .oinstr(oinstr), .oALUout(wALUout), .oDMout(wDMout), .oPC8(wPC8), .oGRF_WD(oGRF_WD));
	
	MUX_4_32bits WMGRF_WS(.in0(wALUout), .in1(wDMout), .in2(wPC8), .isel(iGRF_WS), .out(oGRF_WS));
	
	assign oPC8 = wPC8;
	
endmodule
