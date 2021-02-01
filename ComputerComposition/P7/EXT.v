`timescale 1ns / 1ps

`include "define.v"
module EXT(
		input [15:0] i16,
		input [2:0] iop,
		output [31:0] o32
    );

	reg [31:0] r;
	
	initial r = 32'b0;
	
	always@(*) begin
		if(iop == `EXT_UNSIGNED) r = {16'b0, i16[15:0]};
		else if(iop == `EXT_SIGNED) r = {{16{i16[15]}}, i16[15:0]};
		else if(iop == `EXT_HIGHHALF) r = {i16[15:0], 16'b0};
		else if(iop == `EXT_SHIFTAMT) r = {27'b0, i16[10:6]};
		else r = 32'hf0f0f0f0; // debug
	end
	
	assign o32 = r;

endmodule
