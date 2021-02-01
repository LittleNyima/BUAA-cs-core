`timescale 1ns / 1ps

`include "define.v"
module ALU(
		input [31:0] iA1,
		input [31:0] iA2,
		input [4:0] isa,
		input [4:0] iop,
		output reg [31:0] oresult,
		output reg overflow
    );

	wire wslt = ($signed(iA1) < $signed(iA2));
	wire wsltu = (iA1 < iA2);
	wire [32:0] wsum = {iA1[31], iA1} + {iA2[31], iA2};
	wire [32:0] wdelta = {iA1[31], iA1} - {iA2[31], iA2};
	
	initial begin
		overflow = 1'b0;
		oresult = 32'b0;
	end

	always@(*) begin
		case(iop)
			`ALU_OP_ADD: begin
				oresult = wsum[31:0];
				overflow = ~(wsum[32] == wsum[31]);
			end
			`ALU_OP_SUB: begin
				oresult = wdelta[31:0];
				overflow = ~(wdelta[32] == wdelta[31]);
			end
			`ALU_OP_ADDU: begin 
				oresult = iA1 + iA2;
				overflow = 1'b0;
			end
			`ALU_OP_SUBU: begin 
				oresult = iA1 - iA2;
				overflow = 1'b0;
			end
			`ALU_OP_SLL: begin 
				oresult = iA2 << isa;
				overflow = 1'b0;
			end
			`ALU_OP_SRL: begin 
				oresult = iA2 >> isa;
				overflow = 1'b0;
			end
			`ALU_OP_SRA: begin 
				oresult = ($signed(iA2)) >>> isa;
				overflow = 1'b0;
			end
			`ALU_OP_SLLV: begin 
				oresult = iA2 << iA1[4:0];
				overflow = 1'b0;
			end
			`ALU_OP_SRLV: begin 
				oresult = iA2 >> iA1[4:0];
				overflow = 1'b0;
			end
			`ALU_OP_SRAV: begin 
				oresult = ($signed(iA2)) >>> iA1[4:0];
				overflow = 1'b0;
			end
			`ALU_OP_AND: begin 
				oresult = iA1 & iA2;
				overflow = 1'b0;
			end
			`ALU_OP_OR: begin 
				oresult = iA1 | iA2;
				overflow = 1'b0;
			end
			`ALU_OP_XOR: begin 
				oresult = iA1 ^ iA2;
				overflow = 1'b0;
			end
			`ALU_OP_NOR: begin 
				oresult = ~(iA1 | iA2);
				overflow = 1'b0;
			end
			`ALU_OP_SLT: begin 
				oresult = {31'b0, wslt};
				overflow = 1'b0;
			end
			`ALU_OP_SLTU: begin 
				oresult = {31'b0, wsltu};
				overflow = 1'b0;
			end
		endcase
	end
endmodule
