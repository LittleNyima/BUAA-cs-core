`timescale 1ns / 1ps

module MUX_2_32bits(
	input [31:0] in0,
	input [31:0] in1,
	input isel,
	output [31:0] out
    );

	reg [31:0] r;
	initial r = 32'b0;
	
	always@(*) begin
		if(isel == 1'b0) r = in0;
		else if(isel == 1'b1) r = in1;
	end
	
	assign out = r;

endmodule

module MUX_4_5bits(
	input [4:0] in0,
	input [4:0] in1,
	input [4:0] in2,
	input [4:0] in3,
	input [1:0] isel,
	output [4:0] out
    );

	reg [4:0] r;
	initial r = 5'b0;
	
	always@(*) begin
		if(isel == 2'b00) r = in0;
		else if(isel == 2'b01) r = in1;
		else if(isel == 2'b10) r = in2;
		else if(isel == 2'b11) r = in3;
	end
	
	assign out = r;

endmodule

module MUX_4_32bits(
	input [31:0] in0,
	input [31:0] in1,
	input [31:0] in2,
	input [31:0] in3,
	input [1:0] isel,
	output [31:0] out
    );

	reg [31:0] r;
	initial r = 32'b0;
	
	always@(*) begin
		if(isel == 2'b00) r = in0;
		else if(isel == 2'b01) r = in1;
		else if(isel == 2'b10) r = in2;
		else if(isel == 2'b11) r = in3;
	end
	
	assign out = r;

endmodule

module MUX_8_32bits(
	input [31:0] in0,
	input [31:0] in1,
	input [31:0] in2,
	input [31:0] in3,
	input [31:0] in4,
	input [31:0] in5,
	input [31:0] in6,
	input [31:0] in7,
	input [2:0] isel,
	output [31:0] out
    );

	reg [31:0] r;
	initial r = 32'b0;
	
	always@(*) begin
		if(isel == 3'b000) r = in0;
		else if(isel == 3'b001) r = in1;
		else if(isel == 3'b010) r = in2;
		else if(isel == 3'b011) r = in3;
		else if(isel == 3'b100) r = in4;
		else if(isel == 3'b101) r = in5;
		else if(isel == 3'b110) r = in6;
		else if(isel == 3'b111) r = in7;
	end
	
	assign out = r;

endmodule
