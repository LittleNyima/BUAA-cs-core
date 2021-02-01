`timescale 1ns / 1ps

module IM(
		input [9:0] iPC,
		output [31:0] oinstr
    );
	
	reg [31:0] r [0:1023];
	integer i;
	initial begin
		for(i = 0; i < 1024; i = i + 1) r[i] = 32'b0;
		$readmemh("code.txt", r, 0, 1023);
	end
	assign oinstr = r[iPC];

endmodule
