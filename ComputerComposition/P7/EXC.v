`timescale 1ns / 1ps

module ADELF(
		input [31:0] iPC,
		output reg oEXC
    );

	always@(*) begin
		if(iPC < 32'h3000 || iPC > 32'h4ffc) oEXC = 1'b1;
		else if(iPC[1:0] == 2'b00) oEXC = 1'b0;
		else oEXC = 1'b1;
	end

endmodule

module EXCD(
		input iEXC,
		input [31:0] iPC,
		input clk,
		input reset,
		input enable,
		output reg oEXC,
		output reg [31:0] oPC
	 );
	 
	initial begin
		oEXC = 1'b0;
		oPC = 32'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			oEXC <= 1'b0;
			oPC <= 32'h18231051;
		end
		else if(enable) begin
			oEXC <= iEXC;
			oPC <= {iPC[31:2], 2'b00};
		end
	end
	
endmodule

module EXCE(
		input iEXC,
		input iisBJ,
		input [31:0] iPC,
		input iEXCD,
		input clk,
		input reset,
		input clear,
		output reg oEXC,
		output reg oisBJ,
		output reg [31:0] oPC,
		output reg [4:0] oExcCode
	 );
	
	initial begin
		oEXC = 1'b0;
		oisBJ = 1'b0;
		oPC = 32'b0;
		oExcCode = 5'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			oEXC <= 1'b0;
			oisBJ <= 1'b0;
			oPC <= 32'h18231051;
			oExcCode <= 5'b0;
		end
		else if(clear) begin
			oEXC <= 1'b0;
			//oisBJ <= 1'b0;
			if(~(iPC == 32'h0)) oPC = iPC;
			oExcCode <= 5'b0;
		end
		else begin
			oEXC <= iEXC || iEXCD;
			oisBJ <= iisBJ;
			oPC <= iPC;
			
			if(iEXC) oExcCode <= 5'd10;
			else if(iEXCD) oExcCode <= 5'd4;
			else oExcCode <= 5'd0;
		end
	end
endmodule

module EXCM(
		input ioverflow,
		input iLOADtype,
		input iSAVEtype,
		input iEXCE,
		input iisBJ,
		input [4:0] iExcCode,
		input [31:0] iPC,
		input clk,
		input reset,
		output reg oEXC,
		output reg oisBJ,
		output reg [4:0] oExcCode,
		output reg [31:0] oPC
	 );
	
	initial begin
		oEXC = 1'b0;
		oisBJ = 1'b0;
		oExcCode = 5'b0;
		oPC = 32'h18231051;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			oEXC <= 1'b0;
			oisBJ <= 1'b0;
			oExcCode <= 5'b0;
			oPC <= 32'b0;
		end
		else begin
			oEXC <= iEXCE || ioverflow;
			oisBJ <= iisBJ;
			oPC <= iPC;
			
			if(ioverflow && iLOADtype) oExcCode <= 5'd4;
			else if(ioverflow && iSAVEtype) oExcCode <= 5'd5;
			else if(ioverflow) oExcCode <= 5'd12;
			else if(iEXCE) oExcCode <= iExcCode;
			else oExcCode <= 5'd0;
		end
	end
	
endmodule
