`timescale 1ns / 1ps

`include "define.v"
module ALU(
		input [31:0] iA1,
		input [31:0] iA2,
		input [4:0] isa,
		input [3:0] iop,
		output reg [31:0] oresult
    );

	reg overflow;
	wire wslt = ($signed(iA1) < $signed(iA2));
	wire wsltu = (iA1 < iA2);

	always@(*) begin
		case(iop)
			`ALU_OP_ADD: {overflow, oresult} = iA1 + iA2;
			`ALU_OP_SUB: {overflow, oresult} = iA1 - iA2;
			`ALU_OP_SLL: oresult = iA2 << isa;
			`ALU_OP_SRL: oresult = iA2 >> isa;
			`ALU_OP_SRA: oresult = ($signed(iA2)) >>> isa;
			`ALU_OP_SLLV: oresult = iA2 << iA1[4:0];
			`ALU_OP_SRLV: oresult = iA2 >> iA1[4:0];
			`ALU_OP_SRAV: oresult = ($signed(iA2)) >>> iA1[4:0];
			`ALU_OP_AND: oresult = iA1 & iA2;
			`ALU_OP_OR: oresult = iA1 | iA2;
			`ALU_OP_XOR: oresult = iA1 ^ iA2;
			`ALU_OP_NOR: oresult = ~(iA1 | iA2);
			`ALU_OP_SLT: oresult = {31'b0, wslt};
			`ALU_OP_SLTU: oresult = {31'b0, wsltu};
		endcase
	end
endmodule
