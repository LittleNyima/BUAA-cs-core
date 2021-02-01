`timescale 1ns / 1ps

module BRIDGE(
		input [31:0] iPR_Addr,
		input [31:0] iPR_Data,
		input [31:0] iDEV_Data0,
		input [31:0] iDEV_Data1,
		input [3:0] iPR_BE,
		input [2:0] iINTRQ,
		output [31:0] oPR_Data, // Data to processor
		output [31:0] oDEV_Data, // Data to device
		output [29:0] oDEV_Addr,
		output oDEV_WE0,
		output oDEV_WE1,
		output [5:0] oINTRQ
    );
	
	reg [31:0] rPR_Data;
	reg rDEV_WE0, rDEV_WE1;

	initial begin
		rPR_Data = 32'b0;
		rDEV_WE0 = 1'b0;
		rDEV_WE1 = 1'b0;
	end
	
	assign oDEV_Data = iPR_Data;
	assign oDEV_Addr = iPR_Addr[31:2];
	assign oINTRQ[2:0] = iINTRQ[2:0];
	assign oINTRQ[5:3] = 3'b0;
	assign oPR_Data = rPR_Data;
	assign oDEV_WE0 = rDEV_WE0;
	assign oDEV_WE1 = rDEV_WE1;

	always@(*) begin
		if(iPR_Addr[31:4] == 28'h7f0) begin
			rPR_Data = iDEV_Data0;
			rDEV_WE0 = (iPR_BE == 4'b1111);
			rDEV_WE1 = 1'b0;
		end
		else if(iPR_Addr[31:4] == 28'h7f1) begin
			rPR_Data = iDEV_Data1;
			rDEV_WE0 = 1'b0;
			rDEV_WE1 = (iPR_BE == 4'b1111);
		end
		else begin
			rPR_Data = 32'b0;
			rDEV_WE0 = 1'b0;
			rDEV_WE1 = 1'b0;
		end
	end

endmodule
