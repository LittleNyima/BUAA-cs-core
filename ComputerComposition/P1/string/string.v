`timescale 1ns / 1ps

module string(
		input clk,
		input clr,
		input [7:0] in,
		output out
    );
	 
	 reg [1:0] state = 0;
	 reg ot = 0;
	 assign out = ot;
	 
	 always@(posedge clk, posedge clr) begin
		if(clr) begin
			state <= 0;
			ot <= 0;
		end
		else begin
			if(state == 0) begin
				if(in == "+" || in == "*") begin
					state <= 2'b11;
					ot <= 0;
				end
				else if(in >= "0" && in <= "9") begin
					state <= 2'b01;
					ot <= 1;
				end
			end
			else if(state == 1) begin
				if(in == "+" || in == "*") begin
					state <= 2'b10;
					ot <= 0;
				end
				else if(in >= "0" && in <= "9") begin
					state <= 2'b11;
					ot <= 0;
				end
			end
			else if(state == 2) begin
				if(in == "+" || in == "*") begin
					state <= 2'b11;
					ot <= 0;
				end
				else if(in >= "0" && in <= "9") begin
					state <= 2'b01;
					ot <= 1;
				end
			end
		end
	 end

endmodule
