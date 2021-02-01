`timescale 1ns / 1ps

`include "define.v"
module DMF(
		input [31:0] iAddr,
		input [31:0] iData,
		input [3:0] ictrl,
		output [11:0] oAddr,
		output [31:0] oData,
		output [3:0] oBE
    );

	reg [11:0] rAddr;
	reg [31:0] rData;
	reg [3:0] rBE;
	
	initial begin
		rAddr = 12'b0;
		rData = 32'b0;
		rBE = 4'b0;
	end
	
	always@(*) begin
		rAddr = iAddr[13:2];
		
		if(ictrl == `DM_SB)
			case(iAddr[1:0])
				2'b00: begin
					rData = {24'b0, iData[7:0]};
					rBE = 4'b0001;
				end
				2'b01: begin
					rData = {16'b0, iData[7:0], 8'b0};
					rBE = 4'b0010;
				end
				2'b10: begin
					rData = {8'b0, iData[7:0], 16'b0};
					rBE = 4'b0100;
				end
				2'b11: begin
					rData = {iData[7:0], 24'b0};
					rBE = 4'b1000;
				end
			endcase
		else if(ictrl == `DM_SH)
			case(iAddr[1])
				1'b0: begin
					rData = {16'b0, iData[15:0]};
					rBE = 4'b0011;
				end
				1'b1: begin
					rData = {iData[15:0], 16'b0};
					rBE = 4'b1100;
				end
			endcase
		else if(ictrl == `DM_SW) begin
			rData = iData;
			rBE = 4'b1111;
		end
		else begin
			rData = iData;
			rBE = 4'b0000;
		end
	end
	
	assign oAddr = rAddr;
	assign oData = rData;
	assign oBE = rBE;

endmodule
