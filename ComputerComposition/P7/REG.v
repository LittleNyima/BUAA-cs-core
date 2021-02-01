`timescale 1ns / 1ps

module REGD(
		input [31:0] iinstr,
		input [31:0] iPC8,
		input clk,
		input en,
		input reset,
		output [31:0] oinstr,
		output [31:0] oPC8
    );

	reg [31:0] rinstr, rPC8;
	initial begin
		rinstr = 32'b0;
		rPC8 = 32'h3008;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			rinstr <= 32'b0;
			rPC8 <= 32'h3004;
		end
		else if(en) begin
			rinstr <= iinstr;
			rPC8 <= iPC8;
		end
	end
	
	assign oinstr = rinstr;
	assign oPC8 = rPC8;

endmodule

module REGE(
		input [31:0] iinstr,
		input [31:0] iGRFo1,
		input [31:0] iGRFo2,
		input [31:0] iPC8,
		input [31:0] iEXTout,
		input [7:0] iCMPout,
		input clk,
		input clear,
		input reset,
		output [31:0] oinstr,
		output [31:0] oGRFo1,
		output [31:0] oGRFo2,
		output [31:0] oPC8,
		output [31:0] oEXTout,
		output [7:0] oCMPout
	 );
	 
	reg [31:0] rinstr, rGRFo1, rGRFo2, rPC8, rEXTout;
	reg [7:0] rCMPout;
	initial begin
		rinstr = 32'b0;
		rGRFo1 = 32'b0;
		rGRFo2 = 32'b0;
		rPC8 = 32'h3008;
		rEXTout = 32'b0;
		rCMPout = 8'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			rinstr <= 32'b0;
			rGRFo1 <= 32'b0;
			rGRFo2 <= 32'b0;
			rPC8 <= 32'h3008;
			rEXTout <= 32'b0;
			rCMPout <= 8'b0;
		end
		else if(clear) begin
			rinstr <= 32'b0;
			rGRFo1 <= 32'b0;
			rGRFo2 <= 32'b0;
			rPC8 <= iPC8;
			rEXTout <= 32'b0;
			rCMPout <= 8'b0;
		end
		else begin
			rinstr <= iinstr;
			rGRFo1 <= iGRFo1;
			rGRFo2 <= iGRFo2;
			rPC8 <= iPC8;
			rEXTout <= iEXTout;
			rCMPout <= iCMPout;
		end
	end
	
	assign oinstr = rinstr;
	assign oGRFo1 = rGRFo1;
	assign oGRFo2 = rGRFo2;
	assign oPC8 = rPC8;
	assign oEXTout = rEXTout;
	assign oCMPout = rCMPout;

endmodule

module REGM(
		input [31:0] iinstr,
		input [31:0] iALUout,
		input [31:0] iALUi2,
		input [31:0] iPC8,
		input [31:0] iMDout,
		input [7:0] iCMPout,
		input [4:0] iGRF_WD,
		input clk,
		input reset,
		output [31:0] oinstr,
		output [31:0] oALUout,
		output [31:0] oALUi2,
		output [31:0] oPC8,
		output [31:0] oMDout,
		output [7:0] oCMPout,
		output [4:0] oGRF_WD
	 );
	 
	reg [31:0] rinstr, rALUout, rALUi2, rPC8, rMDout;
	reg [7:0] rCMPout;
	reg [4:0] rGRF_WD;
	initial begin
		rinstr = 32'b0;
		rALUout = 32'b0;
		rALUi2 = 32'b0;
		rPC8 = 32'h3008;
		rMDout = 32'b0;
		rCMPout = 8'b0;
		rGRF_WD = 5'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			rinstr <= 32'b0;
			rALUout <= 32'b0;
			rALUi2 <= 32'b0;
			rPC8 <= 32'h3008;
			rMDout <= 32'b0;
			rCMPout <= 8'b0;
			rGRF_WD <= 5'b0;
		end
		else begin
			rinstr <= iinstr;
			rALUout <= iALUout;
			rALUi2 <= iALUi2;
			rPC8 <= iPC8;
			rMDout <= iMDout;
			rCMPout <= iCMPout;
			rGRF_WD <= iGRF_WD;
		end
	end
	
	assign oinstr = rinstr;
	assign oALUout = rALUout;
	assign oALUi2 = rALUi2;
	assign oPC8 = rPC8;
	assign oMDout = rMDout;
	assign oCMPout = rCMPout;
	assign oGRF_WD = rGRF_WD;
	 
endmodule

module REGW(
		input [31:0] iinstr,
		input [31:0] iALUout,
		input [31:0] iDMout,
		input [31:0] iPC8,
		input [31:0] iMDout,
		input [7:0] iCMPout,
		input [4:0] iGRF_WD,
		input clk,
		input reset,
		output [31:0] oinstr,
		output [31:0] oALUout,
		output [31:0] oDMout,
		output [31:0] oPC8,
		output [31:0] oMDout,
		output [7:0] oCMPout,
		output [4:0] oGRF_WD
	 );
	
	reg [31:0] rinstr, rALUout, rDMout, rPC8, rMDout;
	reg [7:0] rCMPout;
	reg [4:0] rGRF_WD;
	
	initial begin
		rinstr = 32'b0;
		rALUout = 32'b0;
		rDMout = 32'b0;
		rPC8 = 32'h3008;
		rMDout = 32'b0;
		rCMPout = 8'b0;
		rGRF_WD = 5'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			rinstr <= 32'b0;
			rALUout <= 32'b0;
			rDMout <= 32'b0;
			rPC8 <= 32'h3008;
			rMDout <= 32'b0;
			rCMPout <= 8'b0;
			rGRF_WD <= 5'b0;
		end
		else begin
			rinstr <= iinstr;
			rALUout <= iALUout;
			rDMout <= iDMout;
			rPC8 <= iPC8;
			rMDout <= iMDout;
			rCMPout <= iCMPout;
			rGRF_WD <= iGRF_WD;
		end
	end
	
	assign oinstr = rinstr;
	assign oALUout = rALUout;
	assign oDMout = rDMout;
	assign oPC8 = rPC8;
	assign oMDout = rMDout;
	assign oCMPout = iCMPout;
	assign oGRF_WD = rGRF_WD;
	
endmodule
