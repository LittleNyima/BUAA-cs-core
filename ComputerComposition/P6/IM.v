`timescale 1ns / 1ps

module IM(
		input [11:0] iPC,
		output [31:0] oinstr
    );
	
	wire [11:0] PC;
	assign PC = iPC - 12'hc00;
	
	reg [31:0] r [0:4095];
	integer i;
	initial begin
		for(i = 0; i < 4096; i = i + 1) r[i] = 32'b0;
		$readmemh("code.txt", r, 0, 4095);
	end
	assign oinstr = r[PC];

endmodule
