`timescale 1ns / 1ps

module MUX_2_32bits(
		input [31:0] in0,
		input [31:0] in1,
		input in_sel,
		output [31:0] out
    );
	 
	reg [31:0] r;
	assign out = r;
	
	initial r = 32'b0;
	
	always@(*) begin
		if(in_sel == 0) r = in0;
		else r = in1;
	end

endmodule

module MUX_4_5bits(
		input [4:0] in0,
		input [4:0] in1,
		input [4:0] in2,
		input [4:0] in3,
		input [1:0] in_sel,
		output [4:0] out
    );
	 
	reg [4:0] r;
	assign out = r;
	
	initial r = 5'b0;
	
	always@(*) begin
		if(in_sel == 2'b00) r = in0;
		else if(in_sel == 2'b01) r = in1;
		else if(in_sel == 2'b10) r = in2;
		else if(in_sel == 2'b11) r = in3;
	end

endmodule

module MUX_4_32bits(
		input [31:0] in0,
		input [31:0] in1,
		input [31:0] in2,
		input [31:0] in3,
		input [1:0] in_sel,
		output [31:0] out
    );
	 
	reg [31:0] r;
	assign out = r;
	
	initial r = 32'b0;
	
	always@(*) begin
		if(in_sel == 2'b00) r = in0;
		else if(in_sel == 2'b01) r = in1;
		else if(in_sel == 2'b10) r = in2;
		else if(in_sel == 2'b11) r = in3;
	end

endmodule
