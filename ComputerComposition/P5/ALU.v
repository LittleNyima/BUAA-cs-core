`timescale 1ns / 1ps

`include "define.v"
module ALU(
		input [31:0] iA1,
		input [31:0] iA2,
		input [2:0] iop,
		output [31:0] out
    );

	reg [31:0] result;
	reg overflow;
	initial result = 32'b0;
	
	always@(*) begin
		if(iop == `ALU_OP_ADD) {overflow, result} = iA1 + iA2;
		else if(iop == `ALU_OP_SUB) {overflow, result} = iA1 - iA2;
		else if(iop == `ALU_OP_OR) result = iA1 | iA2;
		else {overflow, result} = iA1 + iA2; // default
	end
	
	assign out = result;

endmodule
