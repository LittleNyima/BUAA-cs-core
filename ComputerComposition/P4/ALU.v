`timescale 1ns / 1ps

`ifndef ALU
`define ALU
`define ALU_OP_ADD 2'b00
`define ALU_OP_SUB 2'b01
`define ALU_OP_OR 2'b10
`define ALU_SRC_GRF 1'b0
`define ALU_SRC_EXT 1'b1
`endif

module ALU(
		input [31:0] in_opA,
		input [31:0] in_opB,
		input [1:0] in_option,
		output [31:0] out,
		output out_zero
    );

	reg [31:0] r;
	reg w;
	
	initial begin
		r = 32'b0;
		w = 0;
	end
	
	always@(*) begin
		if(in_option == `ALU_OP_ADD) {w, r} <= in_opA + in_opB;
		else if(in_option == `ALU_OP_SUB) {w, r} <= in_opA - in_opB;
		else if(in_option == `ALU_OP_OR) {w, r} <= in_opA | in_opB;
	end
	
	assign out = r;
	assign out_zero = (in_opA == in_opB);

endmodule
