`timescale 1ns / 1ps

`ifndef EXT
`define EXT
`define EXT_UNSIGNED_EXTEND 2'b00
`define EXT_SIGNED_EXTEND 2'b01
`define EXT_HIGH_EXTEND 2'b10
`endif

module EXT_16TO32(
		input [15:0] in,
		input [1:0] in_op,
		output [31:0] out
    );

	reg [31:0] r;
	
	initial r = 32'b0;
	
	always@(*) begin
		if(in_op == `EXT_UNSIGNED_EXTEND) r = {16'b0, in};
		else if(in_op == `EXT_SIGNED_EXTEND) r = {{16{in[15]}}, in};
		else if(in_op == `EXT_HIGH_EXTEND) r = {in, 16'b0};
	end
	
	assign out = r;

endmodule
