`timescale 1ns / 1ps

`include "define.v"
module DMB(
		input [31:0] iAddr,
		input [31:0] iData,
		input [3:0] ictrl,
		output [31:0] out
    );

	reg [31:0] r;
	initial r = 32'b0;
	
	always@(*) begin
		if(ictrl == `DM_LB)
			case(iAddr[1:0])
				2'b00: r = {{24{iData[7]}}, iData[7:0]};
				2'b01: r = {{24{iData[15]}}, iData[15:8]};
				2'b10: r = {{24{iData[23]}}, iData[23:16]};
				2'b11: r = {{24{iData[31]}}, iData[31:24]};
			endcase
		else if(ictrl == `DM_LBU)
			case(iAddr[1:0])
				2'b00: r = {24'b0, iData[7:0]};
				2'b01: r = {24'b0, iData[15:8]};
				2'b10: r = {24'b0, iData[23:16]};
				2'b11: r = {24'b0, iData[31:24]};
			endcase
		else if(ictrl == `DM_LH)
			case(iAddr[1])
				1'b0: r = {{16{iData[15]}}, iData[15:0]};
				1'b1: r = {{16{iData[31]}}, iData[31:16]};
			endcase
		else if(ictrl == `DM_LHU)
			case(iAddr[1])
				1'b0: r = {16'b0, iData[15:0]};
				1'b1: r = {16'b0, iData[31:16]};
			endcase
		else if(ictrl == `DM_LW) r = iData;
		else r = iData;
	end

	assign out = r;

endmodule
