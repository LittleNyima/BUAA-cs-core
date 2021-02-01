`timescale 1ns / 1ps

module PC(
		input [31:0] iNPC,
		input clk,
		input en,
		input reset,
		output [31:0] oPC
    );
	 
	reg [31:0] rPC;
	 
	initial rPC = 32'h3000;
	 
	always@(posedge clk) begin
		if(reset) rPC <= 32'h3000;
		else if(en) rPC <= iNPC;
	end
	
	assign oPC = rPC;

endmodule
