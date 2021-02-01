`timescale 1ns / 1ps

module DM(
		input [9:0] iAddr,
		input [31:0] iData,
		input [31:0] iPC8,
		input iDM_WE,
		input clk,
		input reset,
		output [31:0] out
    );

	reg [31:0] r [1023:0];
	wire [31:0] PC;
	wire [31:0] Addr;
	
	assign PC = iPC8 - 32'h8;
	assign Addr = {20'b0, iAddr[9:0], 2'b00};
	
	integer i;
	initial begin
		for(i = 0; i < 1024; i = i + 1) r[i] = 32'b0;
	end
	
	always@(posedge clk) begin
		if(reset) for(i = 0; i < 1024; i = i + 1) r[i] <= 32'b0;
		else if(iDM_WE) begin
			r[iAddr] <= iData;
			$display("%d@%h: *%h <= %h", $time, PC, Addr, iData);
			//$display("@%h: *%h <= %h", PC, Addr, iData);
		end
	end
	
	assign out = r[iAddr];

endmodule
