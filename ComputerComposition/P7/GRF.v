`timescale 1ns / 1ps

module GRF(
		input [4:0] iA1_Addr,
		input [4:0] iA2_Addr,
		input [4:0] iA3_Addr,
		input [31:0] iWD,
		input [31:0] iPC8,
		input clk,
		input iWE,
		input reset,
		output [31:0] oA1_Data,
		output [31:0] oA2_Data
    );

	reg [31:0] r [31:0];
	wire [31:0] PC;
	
	assign PC = iPC8 - 32'h8;
	
	integer i;
	initial for(i = 0; i < 32; i = i + 1) r[i] = 32'b0;
	
	always@(posedge clk) begin
		if(reset) for(i = 0; i < 32; i = i + 1) r[i] <= 32'b0;
		else if(iWE && ~(iA3_Addr == 5'b0)) begin
			r[iA3_Addr] <= iWD;
			r[0] <= 32'b0;
			$display("%d@%h: $%d <= %h", $time, PC, iA3_Addr, iWD);
			//$display("@%h: $%d <= %h", PC, iA3_Addr, iWD);
		end
	end
	
	// internal bypass
	assign oA1_Data = (iA1_Addr == 5'b0)? 32'b0 : (iA1_Addr == iA3_Addr && iWE)? iWD : r[iA1_Addr];
	assign oA2_Data = (iA2_Addr == 5'b0)? 32'b0 : (iA2_Addr == iA3_Addr && iWE)? iWD : r[iA2_Addr];

endmodule
