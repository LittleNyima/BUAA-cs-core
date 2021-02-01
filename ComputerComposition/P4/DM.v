`timescale 1ns / 1ps

module DM(
		input [9:0] in_Addr,
		input [31:0] in_Data,
		input [31:0] in_PC4,
		input in_DM_WE,
		input clk,
		input reset,
		output [31:0] out
    );

	reg [31:0] r [1023:0];
	wire [31:0] PC;
	wire [31:0] Addr;
	
	assign PC = in_PC4 - 4;
	assign Addr = {{20{0}}, in_Addr, 2'b00};
	
	integer i;
	initial begin
		for(i = 0; i < 1024; i = i + 1) r[i] = 32'b0;
	end
	
	always@(posedge clk) begin
		if(reset) for(i = 0; i < 1024; i = i + 1) r[i] <= 32'b0;
		else if(in_DM_WE) begin
			r[in_Addr] <= in_Data;
			$display("@%h: *%h <= %h", PC, Addr, in_Data);
		end
	end
	
	assign out = r[in_Addr];

endmodule
