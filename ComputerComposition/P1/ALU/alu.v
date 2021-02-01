`timescale 1ns / 1ps

module alu(
		input [31:0] A,
		input [31:0] B,
		input [2:0] ALUOp,
		output [31:0] C
    );

	reg [31:0] ot;
	assign C = ot;
	
	always@(*) begin
		case (ALUOp)
			3'b000: ot = A + B;
			3'b001: ot = A - B;
			3'b010: ot = A & B;
			3'b011: ot = A | B;
			3'b100: ot = A >> B;
			3'b101: ot = ($signed(A)) >>> B;
			default: ot = {32{0}};
		endcase
	end

endmodule
