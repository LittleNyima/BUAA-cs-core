`timescale 1ns / 1ps

// 	SR[15:10]    - IPL
//		SR[1]        - EXL
// 	SR[0]        - IE
//		CAUSE[31]    - BD
// 	CAUSE[15:10] - RIPL
// 	CAUSE[6:2]   - EXCCODE
//		`define Int  0
//		`define AdEL 4
//		`define AdES 5
//		`define RI   10
//		`define Ov   12

`include "define.v"
module CP0(
		input iAdES,
		input iAdEL,
		input [5:0] iINTRQ,
		input iEXCM,
		input iisBJ,
		input stall,
		input [4:0] iExcCode,
		input [31:0] iPC,
		input [31:0] iPCE,
		input [31:0] iPCD,
		input [31:0] iPCF,
		input [31:0] iData,
		input [4:0] iAddr,
		input iWE,
		input iEXLclr,
		input clk,
		input reset,
		output [31:0] oData,
		output [31:0] oEPC,
		output oINTRQ
    );

	wire [31:0] SR, CAUSE, wEPC;
	wire INT;
	
	reg [15:10] SR_IPL;
	reg SR_EXL, SR_IE;
	
	reg CAUSE_BD;
	reg [15:10] CAUSE_RIPL;
	reg [6:2] CAUSE_EXCCODE;
	
	reg [31:0] EPC, PRID;
	
	reg risBJ, INTRQ;
	
	assign INT = ((|(iINTRQ & SR_IPL)) & SR_IE & ~SR_EXL) || iAdES || iAdEL || iEXCM;
	assign SR = {16'b0, SR_IPL, 8'b0, SR_EXL, SR_IE};
	assign CAUSE = {CAUSE_BD, 15'b0, CAUSE_RIPL, 3'b0, CAUSE_EXCCODE, 2'b0};
	
	assign oData = (iAddr == 5'd12)? SR:
						(iAddr == 5'd13)? CAUSE:
						(iAddr == 5'd14)? EPC:
						(iAddr == 5'd15)? PRID:
						                  32'b0;
	assign oEPC = EPC;
	//assign oINTRQ = INTRQ;
	assign oINTRQ = INT;
	
	/*assign wEPC = (iPC != 32'b0)? iPC :
					  (iPCE != 32'b0)? iPCE :
					  (iPCD != 32'b0)? iPCD :
					  (iPCF != 32'b0)? iPCF :
					  32'b0;*/
					  
	assign wEPC = (iPC != 32'h18231051)? (risBJ? (iPC - 32'h4) : iPC) :
					  (iPCE != 32'h18231051)? (risBJ? (iPCE - 32'h4) : iPCE) :
					  (iPCD != 32'h18231051)? (risBJ? (iPCD - 32'h4) : iPCD) :
					  (iPCF != 32'h18231051)? (risBJ? (iPCF - 32'h4) : iPCF) :
					  32'b0;
	
	initial begin
		SR_IPL = 6'b0;
		SR_EXL = 1'b0;
		SR_IE = 1'b0;
		CAUSE_BD = 1'b0;
		CAUSE_RIPL = 6'b0;
		CAUSE_EXCCODE = 5'b0;
		EPC = 32'b0;
		PRID = 32'h18231051;
		risBJ = 1'b0;
		INTRQ = 1'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			SR_IPL <= 6'b0;
			SR_EXL <= 1'b0;
			SR_IE <= 1'b0;
			CAUSE_BD <= 1'b0;
			CAUSE_RIPL <= 6'b0;
			CAUSE_EXCCODE <= 5'b0;
			EPC <= 32'b0;
			PRID <= 32'h18231051;
			risBJ <= 1'b0;
			INTRQ <= 1'b0;
		end
		else begin
			if(~(iPC == 32'h18231051)) risBJ <= iisBJ;
			if(iWE && ~INT) begin
				case(iAddr)
					5'd12: begin
						SR_IPL <= iData[15:10];
						SR_EXL <= iData[1];
						SR_IE <= iData[0];
						//$display("%d@%h: SR <= %h", $time, iPC, iData);
					end
					/*5'd13: begin
						CAUSE_BD <= iData[31];
						CAUSE_RIPL <= iData[15:10];
						CAUSE_EXCCODE <= iData[6:2];
					end*/
					5'd14: begin 
						EPC <= {iData[31:2] ,2'b00};
						//$display("%d@%h: EPC <= %h", $time, iPC, iData);
					end
					//5'd15: PRID <= iData;
				endcase
			end
			if(INT) begin
				SR_EXL <= 1'b1;
				CAUSE_BD <= risBJ;
				if(iPC != 32'h18231051) EPC <= risBJ? (iPC - 32'h4) : iPC;
				else EPC <= wEPC;
				if(|(iINTRQ & SR_IPL)) CAUSE_EXCCODE <= 5'd0;
				else if(iAdEL) CAUSE_EXCCODE <= 5'd4;
				else if(iAdES) CAUSE_EXCCODE <= 5'd5;
				else if(iEXCM) CAUSE_EXCCODE <= iExcCode;
				else CAUSE_EXCCODE <= 5'd0;
			end
			if(iEXLclr) SR_EXL <= 1'b0;
			CAUSE_RIPL <= iINTRQ;
			INTRQ <= INT;
		end
	end

endmodule
