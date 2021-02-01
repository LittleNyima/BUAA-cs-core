`timescale 1ns / 1ps

module ext(
		input [15:0] imm,
		input [1:0] EOp,
		output [31:0] ext
    );
	 
	reg [31:0] ot;
	assign ext = ot;
	
	always@(*) begin
		case (EOp)
			2'b00: ot = {{16{imm[15]}}, imm[15:0]};
			2'b01: ot = {{16{1'b0}}, imm[15:0]};
			2'b10: ot = {imm[15:0], {16{1'b0}}};
			2'b11: ot = {{16{1'b0}}, imm[15:0]} << 2;
		endcase
	end

endmodule
