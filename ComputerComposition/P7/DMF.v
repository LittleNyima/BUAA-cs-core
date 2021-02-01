`timescale 1ns / 1ps

`include "define.v"
module DMF(
		input [31:0] iAddr,
		input [31:0] iData,
		input [3:0] ictrl,
		input INT,
		output [31:0] oAddr,
		output [31:0] oData,
		output [3:0] oBE,
		output [3:0] oDEV_BE,
		output oEXC
    );

	reg [31:0] rAddr;
	reg [31:0] rData;
	reg [3:0] rBE;
	reg [3:0] rDEV_BE;
	reg rEXC;
	
	initial begin
		rAddr = 32'b0;
		rData = 32'b0;
		rBE = 4'b0;
		rDEV_BE = 4'b0;
		rEXC = 1'b0;
	end
	
	always@(*) begin
		rAddr = iAddr;
		if(INT & ~rEXC) begin
			rBE = 4'b0000;
			rData = iData;
		end
		else if(iAddr < 32'h3000) begin // DM
			rDEV_BE = 4'b0;
			if(ictrl == `DM_SB)
				case(iAddr[1:0])
					2'b00: begin
						rData = {24'b0, iData[7:0]};
						rBE = 4'b0001;
						rEXC = 1'b0;
					end
					2'b01: begin
						rData = {16'b0, iData[7:0], 8'b0};
						rBE = 4'b0010;
						rEXC = 1'b0;
					end
					2'b10: begin
						rData = {8'b0, iData[7:0], 16'b0};
						rBE = 4'b0100;
						rEXC = 1'b0;
					end
					2'b11: begin
						rData = {iData[7:0], 24'b0};
						rBE = 4'b1000;
						rEXC = 1'b0;
					end
				endcase
			else if(ictrl == `DM_SH)
				case(iAddr[1:0])
					2'b00: begin
						rData = {16'b0, iData[15:0]};
						rBE = 4'b0011;
						rEXC = 1'b0;
					end
					2'b10: begin
						rData = {iData[15:0], 16'b0};
						rBE = 4'b1100;
						rEXC = 1'b0;
					end
					default: begin
						rData = iData;
						rBE = 4'b0000;
						rEXC = 1'b1;
					end
				endcase
			else if(ictrl == `DM_SW) begin
				case(iAddr[1:0])
					2'b00: begin
						rData = iData;
						rBE = 4'b1111;
						rEXC = 1'b0;
					end
					default: begin
						rData = iData;
						rBE = 4'b0000;
						rEXC = 1'b1;
					end
				endcase
			end
			else begin
				rData = iData;
				rBE = 4'b0000;
				rEXC = 1'b0;
			end
		end
		else if(iAddr > 32'h7eff && iAddr < 32'h7f08) begin // Timer 0
			rBE = 4'b0;
			if(ictrl == `DM_SB) begin
				rData = iData;
				rDEV_BE = 4'b0000;
				rEXC = 1'b1;
			end
			else if(ictrl == `DM_SH) begin
				rData = iData;
				rDEV_BE = 4'b0000;
				rEXC = 1'b1;
			end
			else if(ictrl == `DM_SW) begin
				if(iAddr[3:2] == 2'b10) begin
					rData = iData;
					rDEV_BE = 4'b0000;
					rEXC = 1'b1;
				end
				else begin
					case(iAddr[1:0])
						2'b00: begin
							rData = iData;
							rDEV_BE = 4'b1111;
							rEXC = 1'b0;
						end
						default: begin
							rData = iData;
							rDEV_BE = 4'b0000;
							rEXC = 1'b1;
						end
					endcase
				end
			end
			else begin
				rData = iData;
				rDEV_BE = 4'b0000;
				rEXC = 1'b0;
			end
		end
		else if(iAddr > 32'h7f0f && iAddr < 32'h7f18) begin // Timer 1
			rBE = 4'b0;
			if(ictrl == `DM_SB) begin
				rData = iData;
				rDEV_BE = 4'b0000;
				rEXC = 1'b1;
			end
			else if(ictrl == `DM_SH) begin
				rData = iData;
				rDEV_BE = 4'b0000;
				rEXC = 1'b1;
			end
			else if(ictrl == `DM_SW) begin
				if(iAddr[3:2] == 2'b10) begin
					rData = iData;
					rDEV_BE = 4'b0000;
					rEXC = 1'b1;
				end
				else begin
					case(iAddr[1:0])
						2'b00: begin
							rData = iData;
							rDEV_BE = 4'b1111;
							rEXC = 1'b0;
						end
						default: begin
							rData = iData;
							rDEV_BE = 4'b0000;
							rEXC = 1'b1;
						end
					endcase
				end
			end
			else begin
				rData = iData;
				rDEV_BE = 4'b0000;
				rEXC = 1'b0;
			end
		end
		else begin // Not Hit
			rBE = 4'b0;
			rDEV_BE = 4'b0;
			rData = iData;
			if(ictrl == `DM_SW || ictrl == `DM_SH || ictrl == `DM_SB) rEXC = 1'b1;
			else rEXC = 1'b0;
		end
	end
	
	assign oAddr = rAddr;
	assign oData = rData;
	assign oBE = rBE & {{4{~INT}}};
	assign oDEV_BE = rDEV_BE & {{4{~INT}}};
	assign oEXC = rEXC;

endmodule
