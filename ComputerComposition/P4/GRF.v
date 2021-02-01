`timescale 1ns / 1ps

module GRF(
		input [4:0] in_rs_Addr,
		input [4:0] in_rt_Addr,
		input [4:0] in_rd_Addr,
		input [31:0] in_GRF_Data,
		input [31:0] in_PC4,
		input in_GRF_WE,
		input clk,
		input reset,
		output [31:0] out_rs_Data,
		output [31:0] out_rt_Data
    );

	reg [31:0] r [31:0];
	wire [31:0] PC;
	
	assign PC = in_PC4 - 4;
	
	integer i;
	initial for(i = 0; i < 32; i = i + 1) r[i] <= 32'b0;
	
	always@(posedge clk) begin
		if(reset) for(i = 0; i < 32; i = i + 1) r[i] <= 32'b0;
		else if(in_GRF_WE) begin
			r[in_rd_Addr] <= in_GRF_Data;
			r[0] <= 32'b0;
			$display("@%h: $%d <= %h", PC, in_rd_Addr, in_GRF_Data);
		end
	end
	
	assign out_rs_Data = r[in_rs_Addr];
	assign out_rt_Data = r[in_rt_Addr];

endmodule
