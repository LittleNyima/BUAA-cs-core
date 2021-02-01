`timescale 1ns / 1ps

module DM(
		input [11:0] iAddr,
		input [31:0] iData,
		input [31:0] iPC8,
		input [3:0] iDM_BE,
		input clk,
		input reset,
		output [31:0] out
    );

	reg [31:0] r [4095:0];
	wire [31:0] PC, Addr, Data;
	wire [7:0] Byte3, Byte2, Byte1, Byte0;
	
	assign PC = iPC8 - 32'h8;
	assign Addr = {18'b0, iAddr[11:0], 2'b00};
	assign Byte3 = (iDM_BE[3])? iData[31:24] : r[iAddr][31:24];
	assign Byte2 = (iDM_BE[2])? iData[23:16] : r[iAddr][23:16];
	assign Byte1 = (iDM_BE[1])? iData[15:8]  : r[iAddr][15:8];
	assign Byte0 = (iDM_BE[0])? iData[7:0]   : r[iAddr][7:0];
	assign Data = {Byte3, Byte2, Byte1, Byte0};
	
	integer i;
	initial begin
		for(i = 0; i < 4096; i = i + 1) r[i] = 32'b0;
	end
	
	always@(posedge clk) begin
		if(reset) for(i = 0; i < 4096; i = i + 1) r[i] <= 32'b0;
		else if(iDM_BE[3] || iDM_BE[2] || iDM_BE[1] || iDM_BE[0]) begin
			r[iAddr] <= Data;
			$display("%d@%h: *%h <= %h", $time, PC, Addr, Data);
			//$display("@%h: *%h <= %h", PC, Addr, Data);
		end
	end
	
	assign out = r[iAddr];

endmodule
