`timescale 1ns / 1ps

`include "define.v"

module CMP(
		input [31:0] iA1,
		input [31:0] iA2,
		output [7:0] oresult
    );
	
	// comment: if add instrction which compare with zero, remember $signed()!!!
	
	assign oresult[`CMP_EQUAL] = (iA1 == iA2);
	assign oresult[`CMP_NEQUAL] = ~(iA1 == iA2);
	assign oresult[`CMP_RSLTZ] = (iA1[31] == 1'b1);
	assign oresult[`CMP_RSLEZ] = (iA1[31] == 1'b1) || (iA1 == 32'b0);
	assign oresult[`CMP_RSGTZ] = (iA1[31] == 1'b0) && ~(iA1 == 32'b0);
	assign oresult[`CMP_RSGEZ] = (iA1[31] == 1'b0);
	//assign oresult[`CMP_RSLTZ] = $signed(iA1) < 32'b0;
	//assign oresult[`CMP_RSLEZ] = ($signed(iA1) < 32'b0) || ($signed(A1) == 32'b0);
	//assign oresult[`CMP_RSGTZ] = $signed(iA1) > 32'b0;
	//assign oresult[`CMP_RSGEZ] = ($signed(iA1) > 32'b0) || ($signed(A1) == 32'b0);
	assign oresult[7:6] = 2'b00;

endmodule
