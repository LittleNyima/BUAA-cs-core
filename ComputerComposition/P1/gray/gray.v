`timescale 1ns / 1ps

module gray(
		input Clk,
		input Reset,
		input En,
		output [2:0] Output,
		output Overflow
    );

	reg [2:0] ot = 3'b000;
	reg ov = 1'b0;
	
	assign Output = ot;
	assign Overflow = ov;
	
	always @(posedge Clk) begin
		if(Reset == 1) begin
			ot <= 3'b000;
			ov <= 1'b0;
		end
		else if(En == 1) begin
			case(ot)
				3'b000: begin
					ot <= 3'b001;
				end
				3'b001: begin
					ot <= 3'b011;
				end
				3'b011: begin
					ot <= 3'b010;
				end
				3'b010: begin
					ot <= 3'b110;
				end
				3'b110: begin
					ot <= 3'b111;
				end
				3'b111: begin
					ot <= 3'b101;
				end
				3'b101: begin
					ot <= 3'b100;
				end
				3'b100: begin
					ot <= 3'b000;
					ov <= 1;
				end
			endcase
		end
	end

endmodule
