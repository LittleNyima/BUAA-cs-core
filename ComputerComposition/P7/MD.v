`timescale 1ns / 1ps

`include "define.v"

module MD(
		input [31:0] iinstr,
		input [31:0] iA1,
		input [31:0] iA2,
		input clk,
		input reset,
		input disab,
		output [31:0] oHI,
		output [31:0] oLO,
		output ooccupy
    );

	wire mult, multu, div, divu;
	wire mfhi, mflo, mthi, mtlo;
	wire [5:0] opcode, func;
	assign opcode = iinstr[31:26];
	assign func = iinstr[5:0];
	assign mult  = (opcode == 6'b000000) && (func == 6'b011000);
	assign multu = (opcode == 6'b000000) && (func == 6'b011001);
	assign div   = (opcode == 6'b000000) && (func == 6'b011010);
	assign divu  = (opcode == 6'b000000) && (func == 6'b011011);
	assign mfhi  = (opcode == 6'b000000) && (func == 6'b010000);
	assign mflo  = (opcode == 6'b000000) && (func == 6'b010010);
	assign mthi  = (opcode == 6'b000000) && (func == 6'b010001);
	assign mtlo  = (opcode == 6'b000000) && (func == 6'b010011);
	
	reg [31:0] bufHI, bufLO, HI, LO;
	reg [3:0] op, count;
	reg busy;
	wire start;
	
	assign start = (mult || multu || div || divu) && ~busy;
	
	initial begin
		bufHI = 32'b0;
		bufLO = 32'b0;
		HI = 32'b0;
		LO = 32'b0;
		op = 4'b0;
		count = 4'b0;
		busy = 1'b0;
	end
	
	always@(posedge clk) begin
		if(reset) begin
			bufHI <= 32'b0;
			bufLO <= 32'b0;
			HI <= 32'b0;
			LO <= 32'b0;
			op <= 4'b0;
			count <= 4'b0;
			busy <= 1'b0;
		end
		else if(((count == 4'b0) || (count == 4'b1)) && disab) begin
			bufHI <= 32'b0;
			bufLO <= 32'b0;
			op <= 4'b0;
			count <= 4'b0;
			busy <= 1'b0;
		end
		else begin
			if(mthi) HI <= iA1;
			else if(mtlo) LO <= iA1;
			if((start == 1'b1) && (busy == 1'b0)) begin
				busy <= 1'b1;
				if(mult) begin
					op <= `MD_OP_MULT;
					count <= 4'b0001;
					{bufHI, bufLO} <= $signed(iA1) * $signed(iA2);
				end
				else if(multu) begin
					op <= `MD_OP_MULTU;
					count <= 4'b0001;
					{bufHI, bufLO} <= iA1 * iA2;
				end
				else if(div) begin
					op <= `MD_OP_DIV;
					count <= 4'b0001;
					if(~(iA2 == 32'b0)) begin
						bufHI <= $signed(iA1) % $signed(iA2);
						bufLO <= $signed(iA1) / $signed(iA2);
					end
					else begin
						//bufHI <= 32'b0;
						//bufLO <= 32'b0;
						bufHI <= HI;
						bufLO <= LO;
					end
				end
				else if(divu) begin
					op <= `MD_OP_DIVU;
					count <= 4'b0001;
					if(~(iA2 == 32'b0)) begin
						bufHI <= iA1 % iA2;
						bufLO <= iA1 / iA2;
					end
					else begin
						//bufHI <= 32'b0;
						//bufLO <= 32'b0;
						bufHI <= HI;
						bufLO <= LO;
					end
				end
			end
			else if(busy == 1'b1) begin
				if(op == `MD_OP_MULT || op == `MD_OP_MULTU) begin
					case(count)
						4'b0001: count <= 4'b0010;
						4'b0010: count <= 4'b0011;
						4'b0011: count <= 4'b0100;
						4'b0100: count <= 4'b0101;
						4'b0101: begin
							busy <= 1'b0;
							count <= 4'b0;
							op <= `MD_OP_NONE;
							HI <= bufHI;
							LO <= bufLO;
							bufHI <= 32'b0;
							bufLO <= 32'b0;
						end
					endcase
				end
				else if(op == `MD_OP_DIV || op == `MD_OP_DIVU) begin
					case(count)
						4'b0001: count <= 4'b0010;
						4'b0010: count <= 4'b0011;
						4'b0011: count <= 4'b0100;
						4'b0100: count <= 4'b0101;
						4'b0101: count <= 4'b0110;
						4'b0110: count <= 4'b0111;
						4'b0111: count <= 4'b1000;
						4'b1000: count <= 4'b1001;
						4'b1001: count <= 4'b1010;
						4'b1010: begin
							busy <= 1'b0;
							count <= 4'b0;
							op <= `MD_OP_NONE;
							HI <= bufHI;
							LO <= bufLO;
							bufHI <= 32'b0;
							bufLO <= 32'b0;
						end
					endcase
				end
			end
		end
	end
	
	assign oHI = HI;
	assign oLO = LO;
	assign ooccupy = start || busy;

endmodule
