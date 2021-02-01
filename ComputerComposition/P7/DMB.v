`timescale 1ns / 1ps

`include "define.v"
module DMB(
		input [31:0] iAddr,
		input [31:0] iData,
		input [31:0] iDEV_Data,
		input [31:0] iCP0_Data,
		input [3:0] ictrl,
		output [31:0] out,
		output oEXC
    );

	reg [31:0] r;
	reg rEXC;
	
	initial begin
		r = 32'b0;
		rEXC = 1'b0;
	end
	
	always@(*) begin
		if(ictrl == `CP0_MFC0) begin
			r = iCP0_Data;
			rEXC = 1'b0;
		end
		else if(iAddr < 32'h3000) begin
			if(ictrl == `DM_LB) begin
				rEXC = 1'b0;
				case(iAddr[1:0])
					2'b00: r = {{24{iData[7]}}, iData[7:0]};
					2'b01: r = {{24{iData[15]}}, iData[15:8]};
					2'b10: r = {{24{iData[23]}}, iData[23:16]};
					2'b11: r = {{24{iData[31]}}, iData[31:24]};
				endcase
			end
			else if(ictrl == `DM_LBU) begin
				rEXC = 1'b0;
				case(iAddr[1:0])
					2'b00: r = {24'b0, iData[7:0]};
					2'b01: r = {24'b0, iData[15:8]};
					2'b10: r = {24'b0, iData[23:16]};
					2'b11: r = {24'b0, iData[31:24]};
				endcase
			end
			else if(ictrl == `DM_LH) begin
				case(iAddr[1:0])
					2'b00: begin
						r = {{16{iData[15]}}, iData[15:0]};
						rEXC = 1'b0;
					end
					2'b10: begin
						r = {{16{iData[31]}}, iData[31:16]};
						rEXC = 1'b0;
					end
					default: begin
						r = iData;
						rEXC = 1'b1;
					end
				endcase
			end
			else if(ictrl == `DM_LHU) begin
				case(iAddr[1:0])
					2'b00: begin
						r = {16'b0, iData[15:0]};
						rEXC = 1'b0;
					end
					2'b10: begin
						r = {16'b0, iData[31:16]};
						rEXC = 1'b0;
					end
					default: begin
						r = iData;
						rEXC = 1'b1;
					end
				endcase
			end
			else if(ictrl == `DM_LW) begin
				case(iAddr[1:0])
					2'b00: begin
						r = iData;
						rEXC = 1'b0;
					end
					default: begin
						r = iData;
						rEXC = 1'b1;
					end
				endcase
			end
			else begin
				r = iData;
				rEXC = 1'b0;
			end
		end
		else if(iAddr > 32'h7eff && iAddr < 32'h7f0c) begin
			if(ictrl == `DM_LB || ictrl == `DM_LBU || ictrl == `DM_LH || ictrl == `DM_LHU) begin
				r = iDEV_Data;
				rEXC = 1'b1;
			end
			else if(ictrl == `DM_LW) begin
				r = iDEV_Data;
				rEXC = 1'b0;
			end
			else begin
				r = iData;
				rEXC = 1'b0;
			end
		end
		else if(iAddr > 32'h7f0f && iAddr < 32'h7f1c) begin
			if(ictrl == `DM_LB || ictrl == `DM_LBU || ictrl == `DM_LH || ictrl == `DM_LHU) begin
				r = iDEV_Data;
				rEXC = 1'b1;
			end
			else if(ictrl == `DM_LW) begin
				r = iDEV_Data;
				rEXC = 1'b0;
			end
			else begin
				r = iData;
				rEXC = 1'b0;
			end
		end
		else begin
			r = iData;
			if(ictrl == `DM_LB || ictrl == `DM_LBU || ictrl == `DM_LH || ictrl == `DM_LHU || ictrl == `DM_LW) rEXC = 1'b1;
			else rEXC = 1'b0;
		end
	end

	assign out = r;
	assign oEXC = rEXC;

endmodule
