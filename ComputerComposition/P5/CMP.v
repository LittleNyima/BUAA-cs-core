`timescale 1ns / 1ps

module CMP(
		input [31:0] iA1,
		input [31:0] iA2,
		output oequal
    );

	assign oequal = (iA1 == iA2);
	
	// comment: if add instrction which compare with zero, remember $signed()!!!

endmodule
