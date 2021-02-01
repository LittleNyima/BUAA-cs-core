`timescale 1ns / 1ps

`ifndef IFU
`define IFU
`define IFU_SRC_INSTR 1'b0
`define IFU_SRC_GRF 1'b1
`define NPC_SEL_SEQUENCE 2'b00
`define NPC_SEL_BRANCHEQ 2'b01
`define NPC_SEL_JUMP 2'b10
`define NPC_SEL_JUMPREG 2'b11
`define TEXT_BASE 32'h3000
`endif

module IFU(
		input [1:0] in_nPC_sel,
		input in_zero,
		input [31:0] in32,
		input clk,
		input reset,
		output [31:0] out_instr,
		output [31:0] out_PC4
    );

	reg [31:0] PC;
	
	initial PC <= `TEXT_BASE;
	
	always@(posedge clk) begin
		if(reset) PC <= `TEXT_BASE;
		else if(in_nPC_sel == `NPC_SEL_SEQUENCE) PC <= PC +4;
		else if(in_nPC_sel == `NPC_SEL_BRANCHEQ)
			if(in_zero == 1) PC <= PC + 4 + {{14{in32[15]}}, in32[15:0], 2'b00};
			else PC <= PC + 4;
		else if(in_nPC_sel == `NPC_SEL_JUMP) PC <= {PC[31:28], in32[25:0], 2'b00};
		else if(in_nPC_sel == `NPC_SEL_JUMPREG) PC <= in32;
	end
	
	ROM_1024_32bits RROM(.in_Addr(PC[11:2]), .out(out_instr));
	assign out_PC4 = PC + 4;

endmodule

module ROM_1024_32bits(
		input [9:0] in_Addr,
		output [31:0] out
	 );
	 
	reg [31:0] r [0:1023];
	integer i;
	initial begin
		for(i = 0; i < 1024; i = i + 1) r[i] = 32'b0;
		$readmemh("code.txt", r, 0, 1023);
	end
	assign out = r[in_Addr];

endmodule
